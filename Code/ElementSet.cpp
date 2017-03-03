// ElementSet.cpp

#include "ElementSet.h"

//------------------------------------------------------------------------------------------
//                                         Element
//------------------------------------------------------------------------------------------

Element::Element( void )
{
}

/*virtual*/ Element::~Element( void )
{
}

//------------------------------------------------------------------------------------------
//									       ElementSet
//------------------------------------------------------------------------------------------

ElementSet::ElementSet( void )
{
}

/*virtual*/ ElementSet::~ElementSet( void )
{
	Clear();
}

/*virtual*/ Element* ElementSet::Multiply( const Element* elementA, const Element* elementB ) const
{
	return elementA->MultiplyOnRight( elementB );
}

/*virtual*/ Element* ElementSet::Invert( const Element* element ) const
{
	return element->Invert();
}

/*virtual*/ bool ElementSet::AreEqual( const Element* elementA, const Element* elementB ) const
{
	return elementA->IsEqualTo( elementB );
}

/*virtual*/ ElementSet* ElementSet::Clone( void ) const
{
	ElementSet* set = New();

	for( ElementList::const_iterator iter = elementList.cbegin(); iter != elementList.cend(); iter++ )
	{
		const Element* element = *iter;
		set->elementList.push_back( element->Clone() );
	}

	return set;
}

bool ElementSet::GenerateGroupResursive( const ElementSet& generatorSet, std::ostream* ostream /*= nullptr*/ )
{
	if( generatorSet.Cardinality() == 0 )
	{
		Clear();
		AddNewMember( Identity() );
		return true;
	}

	std::auto_ptr< ElementSet > subGeneratorSet( generatorSet.Clone() );

	ElementList::iterator iter = subGeneratorSet->elementList.begin();
	Element* lastGenerator = *iter;
	subGeneratorSet->elementList.erase( iter );

	std::auto_ptr< ElementSet > subGroup( New() );
	if( !subGroup->GenerateGroupResursive( *subGeneratorSet, ostream ) )
		return false;

	Clear();

	uint order = lastGenerator->Order();

	Element* multiplierElement = nullptr;

	for( uint i = 0; i < order; i++ )
	{
		for( iter = subGroup->elementList.begin(); iter != subGroup->elementList.end(); iter++ )
		{
			Element* newElement = ( *iter )->Clone();

			if( multiplierElement )
			{
				Element* deleteMe = newElement;
				newElement = Multiply( multiplierElement, deleteMe );
				delete deleteMe;
			}

			// This is what I'm hoping is a major optimization.
			// I know that I can unconditionally add these elements to the set,
			// because I know that they're all unique.
			elementList.push_back( newElement );
		}

		if( !multiplierElement )
			multiplierElement = lastGenerator->Clone();
		else
		{
			Element* deleteMe = multiplierElement;
			multiplierElement = Multiply( lastGenerator, deleteMe );
			delete deleteMe;
		}
	}

	delete multiplierElement;

	// Interestingly, if we knew the subgroup was normal,
	// then this last step would be unecessary.  This is where
	// we're sure to be spending most of our time.  Fortunately,
	// if the subgroup really is normal, this should be very fast.

	ElementSet* elementQueue = New();
	elementQueue->AddNewMember( lastGenerator );

	ProcessElementQueue( elementQueue, ostream );
	delete elementQueue;

	return true;
}

bool ElementSet::GenerateGroup( const ElementSet& generatorSet, std::ostream* ostream /*= nullptr*/ )
{
	ElementSet* elementQueue = New();

	for( ElementList::const_iterator iter = generatorSet.elementList.cbegin(); iter != generatorSet.elementList.cend(); iter++ )
		elementQueue->elementList.push_back( ( *iter )->Clone() );
	
	Clear();
	AddNewMember( Identity() );

	ProcessElementQueue( elementQueue, ostream );
	delete elementQueue;

	return true;
}

/*

Here's what might be a better idea (and maybe faster?!) for generating a group.
The idea is to grow a Caylay table.  The generators form the row and column headers
initially.  We fill in the table, then discover new elements in the middle that
aren't in any header.  So we put those in row and column headers, then go fill in
the new blank spots in the larger Caylay table.  This might be faster, because we
are avoiding doing any multiplications redundantly.  We still have the problem of
having to do a linear search in the set to check membership and do insertion, but
with eliminating any unnecessary multiplications, we may also reduce the number of
such searces.

Actually, there is no advantage to this.  But it would lend itself well to being
farmed out to multiple threads!  Another optimization that can be made is that
when we're checking that a newly calculated element in the Caylay table is not
in a row (or column) header, we don't have to check against the header elements
that match was is in the row or column of the new element.  So for each slot in
the Caylay table, we could maintain the subset of header elements we need to check
against.  Coming up with those initial subsets can be done using hash table lookups.

*/

bool ElementSet::ProcessElementQueue( ElementSet* elementQueue, std::ostream* ostream /*= nullptr*/ )
{
	while( elementQueue->Cardinality() > 0 )
	{
		ElementList::iterator queueIter = elementQueue->elementList.begin();
		Element* newElement = *queueIter;
		elementQueue->elementList.erase( queueIter );

		if( ostream )
		{
			*ostream << "QueueSize: " << elementQueue->Cardinality() << "\n";
			*ostream << "GroupSize: " << Cardinality() << "\n";
		}

		for( ElementList::const_iterator iter = elementList.cbegin(); iter != elementList.cend(); iter++ )
		{
			const Element* oldElement = *iter;

			for( int i = 0; i < 2; i++ )
			{
				Element* product = nullptr;

				if( i == 0 )
					product = Multiply( newElement, oldElement );
				else
					product = Multiply( oldElement, newElement );

				bool foundInSet = IsMember( product );
				bool foundInQueue = elementQueue->IsMember( product );

				if( !( foundInSet || foundInQueue ) )
					elementQueue->AddNewMember( product );
				else
					delete product;
			}
		}

		if( !AddNewMember( newElement ) )
			delete newElement;
		else if( ostream )
			newElement->Print( *ostream );
	}

	return true;
}

uint ElementSet::Cardinality( void ) const
{
	return elementList.size();
}

void ElementSet::Clear( void )
{
	DeleteList( elementList );
}

/*static*/ void ElementSet::DeleteList( ElementList& elementList )
{
	while( elementList.size() > 0 )
	{
		ElementList::iterator iter = elementList.begin();
		Element* element = *iter;
		delete element;
		elementList.erase( iter );
	}
}

void ElementSet::Print( std::ostream& ostream ) const
{
	for( ElementList::const_iterator iter = elementList.cbegin(); iter != elementList.cend(); iter++ )
	{
		const Element* member = *iter;
		member->Print( ostream );
	}
}

bool ElementSet::IsMember( const Element* element, ElementList::iterator* foundIter /*= nullptr*/ )
{
	// This linear search is the main innefficiency in my design.
	// If elements have a unique representation, then we can use a hash table to get better performance.
	// In the case of factor groups, however, I do not know how to do better than this linear search.
	for( ElementList::iterator iter = elementList.begin(); iter != elementList.end(); iter++ )
	{
		Element* member = *iter;
		if( AreEqual( member, element ) )
		{
			if( foundIter )
				*foundIter = iter;
			return true;
		}
	}

	return false;
}

bool ElementSet::AddNewMember( Element* element )
{
	if( IsMember( element ) )
		return false;

	elementList.push_back( element );
	return true;
}

//------------------------------------------------------------------------------------------
//                                  PermutationElement
//------------------------------------------------------------------------------------------

PermutationElement::PermutationElement( void )
{
}

/*virtual*/ PermutationElement::~PermutationElement( void )
{
}

/*virtual*/ Element* PermutationElement::Clone( void ) const
{
	PermutationElement* element = new PermutationElement();
	element->word.SetCopy( word );
	element->permutation.SetCopy( permutation );
	return element;
}

/*virtual*/ uint PermutationElement::Order( void ) const
{
	return permutation.Order();
}

/*virtual*/ std::string PermutationElement::Name( void ) const
{
	if( word.termList.size() != 1 )
		return "";

	const Term& term = word.termList.front();
	return term.name;
}

/*virtual*/ void PermutationElement::Print( std::ostream& ostream ) const
{
	word.Print( ostream );
	ostream << " = ";
	permutation.Print( ostream );
	ostream << "\n";
}

//------------------------------------------------------------------------------------------
//                                     PermutationSet
//------------------------------------------------------------------------------------------

PermutationSet::PermutationSet( void )
{
}

/*virtual*/ PermutationSet::~PermutationSet( void )
{
}

/*virtual*/ ElementSet* PermutationSet::New( void ) const
{
	return new PermutationSet();
}

/*virtual*/ Element* PermutationSet::Identity( void ) const
{
	return new PermutationElement();
}

/*virtual*/ Element* PermutationSet::Multiply( const Element* elementA, const Element* elementB ) const
{
	const PermutationElement* permElementA = dynamic_cast< const PermutationElement* >( elementA );
	const PermutationElement* permElementB = dynamic_cast< const PermutationElement* >( elementB );

	if( !( permElementA && permElementB ) )
		return nullptr;

	PermutationElement* product = new PermutationElement();

	product->word.Multiply( permElementA->word, permElementB->word );
	product->permutation.Multiply( permElementA->permutation, permElementB->permutation );

	return product;
}

/*virtual*/ Element* PermutationSet::Invert( const Element* element ) const
{
	const PermutationElement* permElement = dynamic_cast< const PermutationElement* >( element );
	if( !permElement )
		return nullptr;

	PermutationElement* invPermElement = new PermutationElement();

	invPermElement->word.SetInverse( permElement->word );
	invPermElement->permutation.SetInverse( permElement->permutation );

	return invPermElement;
}

/*virtual*/ bool PermutationSet::AreEqual( const Element* elementA, const Element* elementB ) const
{
	const PermutationElement* permElementA = dynamic_cast< const PermutationElement* >( elementA );
	const PermutationElement* permElementB = dynamic_cast< const PermutationElement* >( elementB );

	if( !( permElementA && permElementB ) )
		return false;

	return permElementA->permutation.IsEqualTo( permElementB->permutation );
}

//------------------------------------------------------------------------------------------
//                                 CosetRepresentativeSet
//------------------------------------------------------------------------------------------

CosetRepresentativeSet::CosetRepresentativeSet( void )
{
}

/*virtual*/ CosetRepresentativeSet::~CosetRepresentativeSet( void )
{
}

/*virtual*/ ElementSet* CosetRepresentativeSet::New( void ) const
{
	return new CosetRepresentativeSet();
}

bool CosetRepresentativeSet::IsInDivsorGroup( const Permutation& permutation ) const
{
	NaturalNumberSet permUnstableSet;
	permutation.GetUnstableSet( permUnstableSet );

	return permUnstableSet.IsSubsetOf( unstableSet );
}

/*virtual*/ bool CosetRepresentativeSet::AreEqual( const Element* elementA, const Element* elementB ) const
{
	const PermutationElement* permElementA = dynamic_cast< const PermutationElement* >( elementA );
	const PermutationElement* permElementB = dynamic_cast< const PermutationElement* >( elementB );

	if( !( permElementA && permElementB ) )
		return false;

	Permutation invPermA;
	invPermA.SetInverse( permElementA->permutation );

	Permutation product;
	product.Multiply( invPermA, permElementB->permutation );

	return IsInDivsorGroup( product );
}

// ElementSet.cpp