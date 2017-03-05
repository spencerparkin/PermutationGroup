// ElementSet.cpp

#include "ElementSet.h"
#include <cstdlib>

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

	for( uint i = 0; i < elementArray.size(); i++ )
	{
		const Element* member = elementArray[i];
		set->elementArray.push_back( member->Clone() );
	}

	return set;
}

uint ElementSet::RandomInteger( uint min, uint max )
{
	float t = float( std::rand() ) / float( RAND_MAX );
	float r = float( min ) + t * float( max - min );
	uint i = uint(r);
	if( i < min )
		i = min;
	if( i > max )
		i = max;
	return i;
}

bool ElementSet::GenerateGroup( const ElementSet& generatorSet, std::ostream* ostream /*= nullptr*/, bool multiThreaded /*= true*/ )
{
	Clear();
	for( uint i = 0; i < generatorSet.elementArray.size(); i++ )
		AddNewMember( generatorSet.elementArray[i]->Clone() );

	if( generatorSet.Cardinality() == 0 )
		return true;

	std::srand(0);

	typedef std::list< CaylayColumn* > CaylayColumnList;
	CaylayColumnList caylayColumnList;

	// Arbitrarily pick an element to use as our first caylay column.
	CaylayColumn* caylayColumn = new CaylayColumn();
	caylayColumn->permuterElement = elementArray[0];
	caylayColumnList.push_back( caylayColumn );

	uint stagnationCount = 0;

	while( true )
	{
		uint oldOrder = elementArray.size();

		// Grow all caylay columns until no one column grows.
		uint growCount = 0;
		uint maxGrowth;
		while( true )
		{	
			maxGrowth = 0;
			for( CaylayColumnList::iterator iter = caylayColumnList.begin(); iter != caylayColumnList.end(); iter++ )
			{
				CaylayColumn* caylayColumn = *iter;
				uint growth = caylayColumn->Grow( this );
				if( growth > maxGrowth )
					maxGrowth = growth;
			}
			if( maxGrowth > 0 )
				growCount++;
			else
				break;
		}

		uint newOrder = elementArray.size();

		if( newOrder == oldOrder )
			stagnationCount++;
		else
			stagnationCount = 0;

		if( ostream )
		{
			*ostream << "Size: " << elementArray.size() << "\n";
			*ostream << "Columns: " << caylayColumnList.size() << "\n";
		}

		// After growing all columns, if we grew as many as there are known elements of the group,
		// then we have generated the entire group.  This is unlikely for large groups, but may happen
		// if we're trying to generate a very small group.
		if( caylayColumnList.size() == elementArray.size() )
			break;

		// Terminate now as an approximation of having generated the group if we were stagnet long enough.
		// In many cases, we have generated the entire group, and we can add more columns to our heart's
		// content and it will never generate any new elements.
		if( stagnationCount > 15 )		// I'm not sure what this should be.
			break;

		// Pick an element at random to start a new column.
		while( true )
		{
			uint i = RandomInteger( 0, elementArray.size() - 1 );
			CaylayColumnList::iterator iter = caylayColumnList.begin();
			while( iter != caylayColumnList.end() )
			{
				CaylayColumn* caylayColumn = *iter;
				if( caylayColumn->permuterElement == elementArray[i] )
					break;
				iter++;
			}
			if( iter == caylayColumnList.end() )
			{
				CaylayColumn* caylayColumn = new CaylayColumn();
				caylayColumn->permuterElement = elementArray[i];
				caylayColumnList.push_back( caylayColumn );
				break;
			}
		}
	}

	while( caylayColumnList.size() > 0 )
	{
		CaylayColumnList::iterator iter = caylayColumnList.begin();
		CaylayColumn* caylayColumn = *iter;
		delete caylayColumn;
		caylayColumnList.erase( iter );
	}

	return true;
}

uint ElementSet::CaylayColumn::Grow( ElementSet* set )
{
	uint growth = 0;

	while( columnArray.size() < set->elementArray.size() )
	{
		Element* element = set->elementArray[ columnArray.size() ];
		Element* product = set->Multiply( element, permuterElement );

		uint offset;
		if( !set->IsMember( product, &offset ) )
			set->elementArray.push_back( product );
		else
		{
			delete product;
			product = set->elementArray[ offset ];
		}

		columnArray.push_back( product );
		growth++;
	}

	return growth;
}

uint ElementSet::Cardinality( void ) const
{
	return elementArray.size();
}

void ElementSet::Clear( void )
{
	for( uint i = 0; i < elementArray.size(); i++ )
	{
		Element* member = elementArray[i];
		delete member;
	}
}

void ElementSet::Print( std::ostream& ostream ) const
{
	for( uint i = 0; i < elementArray.size(); i++ )
	{
		const Element* member = elementArray[i];
		member->Print( ostream );
	}
}

// This linear search is the main innefficiency in the program.
// If elements have a unique representation, then we can use a hash table to get better performance.
// In the case of factor groups, however, I do not know how to do better than a linear search.
bool ElementSet::IsMember( const Element* element, uint* offset /*= nullptr*/ )
{
	// Creating and destroy threads rappedly might be too costly.
	// So we don't go multithreded unless we really need to.
	uint maxElementsPerThread = 1000;

	//if( elementArray.size() <= maxElementsPerThread )
	if( true )
	{
		for( uint i = 0; i < elementArray.size(); i++ )
		{
			Element* member = elementArray[i];
			if( AreEqual( member, element ) )
			{
				if( offset )
					*offset = i;
				return true;
			}
		}
	}
	else
	{
		MembershipThreadList threadList;
		uint i = 0;

		uint threadCount = ( uint )ceil( float( elementArray.size() ) / float( maxElementsPerThread ) );
		uint elementsPerThread = elementArray.size() / threadCount + 1;

		do
		{
			MembershipThread* thread = new MembershipThread();
			thread->thread = nullptr;
			thread->found = false;
			thread->done = false;
			thread->abort = false;
			thread->element = element;
			thread->set = this;
			thread->i = 0;
			thread->min = i;
			thread->max = ( ( i + elementsPerThread ) <= elementArray.size() ) ? ( i + elementsPerThread ) : elementArray.size();
			i += elementsPerThread;
			threadList.push_back( thread );
		}
		while( i < elementArray.size() );

		uint concurrency = std::thread::hardware_concurrency();

		MembershipThreadList activeThreadList;
		bool found = false;

		while( !found && ( threadList.size() > 0 || activeThreadList.size() > 0 ) )
		{
			while( activeThreadList.size() < concurrency && threadList.size() > 0 )
			{
				MembershipThreadList::iterator iter = threadList.begin();
				MembershipThread* thread = *iter;
				threadList.erase( iter );
				thread->thread = new std::thread( &ElementSet::MembershipThread::FindMember, thread );
				activeThreadList.push_back( thread );
			}

			for( MembershipThreadList::iterator iter = activeThreadList.begin(); iter != activeThreadList.end(); iter++ )
			{
				MembershipThread* thread = *iter;
				if( thread->done )
				{
					if( thread->found )
					{
						found = true;
						if( offset )
							*offset = thread->i;
					}

					thread->thread->join();
					delete thread->thread;
					delete thread;
					activeThreadList.erase( iter );
					break;
				}
			}
		}

		for( MembershipThreadList::iterator iter = activeThreadList.begin(); iter != activeThreadList.end(); iter++ )
			( *iter )->abort = true;
		
		while( threadList.size() > 0 )
		{
			MembershipThreadList::iterator iter = threadList.begin();
			MembershipThread* thread = *iter;
			threadList.erase( iter );
			delete thread;
		}

		while( activeThreadList.size() > 0 )
		{
			MembershipThreadList::iterator iter = activeThreadList.begin();
			MembershipThread* thread = *iter;
			activeThreadList.erase( iter );
			thread->thread->join();
			delete thread->thread;
			delete thread;
		}

		return found;
	}

	return false;
}

void ElementSet::MembershipThread::FindMember( void )
{
	i = min;
	while( i < max && !abort )
	{
		Element* member = set->elementArray[i];
		if( set->AreEqual( member, element ) )
		{
			found = true;
			break;
		}
		i++;
	}

	done = true;
}

bool ElementSet::AddNewMember( Element* element )
{
	if( IsMember( element ) )
		return false;

	elementArray.push_back( element );
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