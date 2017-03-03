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

bool ElementSet::GenerateGroup( const ElementSet& generatorSet, std::ostream* ostream /*= nullptr*/, bool multiThreaded /*= true*/ )
{
	std::auto_ptr< CaylayTableData > data( new CaylayTableData() );

	for( ElementList::const_iterator iter = generatorSet.elementList.cbegin(); iter != generatorSet.elementList.cend(); iter++ )
		data->caylayTableHeaderArray.push_back( ( *iter )->Clone() );

	for( uint i = 0; i < data->caylayTableHeaderArray.size(); i++ )
	{
		ElementHashMap* elementHashMap = new ElementHashMap();
		data->caylayColumnCheckArray.push_back( elementHashMap );

		for( uint j = 0; j < data->caylayTableHeaderArray.size(); j++ )
			elementHashMap->insert( data->caylayTableHeaderArray[j] );
	}

	uint maxThreadCount = std::thread::hardware_concurrency();

	CaylayBlock block;
	block.minRow = 0;
	block.maxRow = data->caylayTableHeaderArray.size() - 1;
	block.minCol = 0;
	block.maxCol = data->caylayTableHeaderArray.size() - 1;

	CaylayBlockList blockList;
	blockList.push_back( block );

	while( true )
	{
		ChopUpBlockList( blockList, maxThreadCount, 25 );

		uint oldGroupOrder = data->caylayTableHeaderArray.size();

		if( *ostream )
			*ostream << "Current group order: " << oldGroupOrder << "\n";
		
		std::list< Thread* > threadList;

		for( CaylayBlockList::iterator iter = blockList.begin(); iter != blockList.end(); iter++ )
		{
			Thread* thread = new Thread();
			thread->set = this;
			thread->data = data.get();
			thread->block = *iter;
			thread->thread = nullptr;
			threadList.push_back( thread );
		}

		if( multiThreaded )
		{
			if( ostream )
				*ostream << "Kicking off " << threadList.size() << " thread(s)!\n";

			for( std::list< Thread* >::iterator iter = threadList.begin(); iter != threadList.end(); iter++ )
			{
				Thread* thread = *iter;
				thread->thread = new std::thread( &ElementSet::Thread::Generate, thread );
			}
		}
		else
		{
			for( std::list< Thread* >::iterator iter = threadList.begin(); iter != threadList.end(); iter++ )
			{
				Thread* thread = *iter;
				thread->Generate();
			}
		}

		while( threadList.size() > 0 )
		{
			std::list< Thread* >::iterator iter = threadList.begin();
			Thread* thread = *iter;
			if( multiThreaded )
				thread->thread->join();
			delete thread->thread;
			delete thread;
			threadList.erase( iter );

			if( ostream && multiThreaded )
				*ostream << "Thread joined.  " << threadList.size() << " thread(s) remaining.\n";
		}

		uint newGroupOrder = data->caylayTableHeaderArray.size();

		if( newGroupOrder == oldGroupOrder )
		{
			if( ostream )
				*ostream << "Group order: " << newGroupOrder << "\n";
			break;
		}
		else
		{
			blockList.clear();

			block.minRow = oldGroupOrder;
			block.maxRow = newGroupOrder - 1;
			block.minCol = 0;
			block.maxCol = oldGroupOrder;
			blockList.push_back( block );

			block.minRow = 0;
			block.maxRow = newGroupOrder - 1;
			block.minCol = oldGroupOrder;
			block.maxCol = newGroupOrder - 1;
			blockList.push_back( block );

			for( uint i = 0; i < newGroupOrder; i++ )
			{
				uint k = oldGroupOrder;
				if( i >= data->caylayColumnCheckArray.size() )
				{
					data->caylayColumnCheckArray.push_back( new ElementHashMap() );
					k = 0;
				}

				ElementHashMap* elementHashMap = data->caylayColumnCheckArray[i];
				for( uint j = k; j < newGroupOrder; j++ )
					elementHashMap->insert( data->caylayTableHeaderArray[j] );
			}
		}
	}

	Clear();

	for( uint i = 0; i < data->caylayTableHeaderArray.size(); i++ )
		elementList.push_back( data->caylayTableHeaderArray[i] );

	for( uint i = 0; i < data->caylayColumnCheckArray.size(); i++ )
		delete data->caylayColumnCheckArray[i];

	return true;
}

// TODO: We might get better net performance by allowing horizontal splits and then adding more locking?
void ElementSet::ChopUpBlockList( CaylayBlockList& blockList, uint maxThreadCount, uint minBlockSize )
{
	// Notice that we do not let any block contain an element of the table
	// that sits above or below any element in any other block.  This is
	// so that we don't have to share the column check array resource among threads.
	while( blockList.size() < maxThreadCount )
	{
		uint maxArea = 0;
		CaylayBlockList::iterator foundIter = blockList.end();
		for( CaylayBlockList::iterator iter = blockList.begin(); iter != blockList.end(); iter++ )
		{
			uint area = ( *iter ).Area();
			if( area > maxArea )
			{
				maxArea = area;
				foundIter = iter;
			}
		}

		if( maxArea <= minBlockSize )
			break;

		const CaylayBlock& block = *foundIter;
		if( block.Width() < 4 )
			break;
	
		CaylayBlock leftBlock, rightBlock;

		uint halfWidth = block.Width() / 2;

		leftBlock.minRow = block.minRow;
		leftBlock.maxRow = block.maxRow;
		leftBlock.minCol = block.minCol;
		leftBlock.maxCol = block.minCol + halfWidth;

		rightBlock.minRow = block.minRow;
		rightBlock.maxRow = block.maxRow;
		rightBlock.minCol = leftBlock.maxCol + 1;
		rightBlock.maxCol = block.maxCol;

		blockList.erase( foundIter );
		blockList.push_back( leftBlock );
		blockList.push_back( rightBlock );
	}
}

void ElementSet::Thread::Generate( void )
{
	for( uint i = block.minRow; i <= block.maxRow; i++ )
	{
		const Element* elementA = data->caylayTableHeaderArray[i];

		for( uint j = block.minCol; j <= block.maxCol; j++ )
		{
			const Element* elementB = data->caylayTableHeaderArray[j];

			Element* product = set->Multiply( elementA, elementB );

			ElementHashMap* elementHashMap = data->caylayColumnCheckArray[j];

			// This is the painful linear search we must do because elements of the group do not necessarily have unique representation.
			Element* foundElement = nullptr;
			for( ElementHashMap::iterator iter = elementHashMap->begin(); iter != elementHashMap->end(); iter++ )
			{
				Element* element = ( *iter ).element;
				if( set->AreEqual( product, element ) )
				{
					foundElement = element;
					break;
				}
			}

			if( foundElement )
			{
				delete product;

				// For the remainder of the algorithm, if any product is computed in this column,
				// we don't have to do our linear search against this element!  This is exploiting
				// the property of every Caylay table that no element will appear more than once
				// in any row or column.  I don't think we're fully exploiting this property, but
				// we're at least not ignoring it.
				elementHashMap->erase( foundElement );
			}
			else
			{
				std::lock_guard< std::mutex > guard( data->caylayTableHeaderArrayMutex );
				data->caylayTableHeaderArray.push_back( product );
			}
		}
	}
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