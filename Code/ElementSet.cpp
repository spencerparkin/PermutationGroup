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

bool ElementSet::GenerateGroup( std::ostream* ostream /*= nullptr*/ )
{
	ElementSet* elementQueue = Clone();
	
	Clear();

	AddNewMember( Identity() );

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