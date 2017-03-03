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

	Clear();
	for( ElementList::const_iterator iter = generatorSet.elementList.cbegin(); iter != generatorSet.elementList.cend(); iter++ )
		elementList.push_back( ( *iter )->Clone() );

	for( ElementList::iterator iter = elementList.begin(); iter != elementList.end(); iter++ )
		data->caylayTableHeaderArray.push_back( *iter );

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
		
		std::list< CaylayTableThread* > tableThreadList;

		for( CaylayBlockList::iterator iter = blockList.begin(); iter != blockList.end(); iter++ )
		{
			CaylayTableThread* thread = new CaylayTableThread();
			thread->set = this;
			thread->data = data.get();
			thread->block = *iter;
			thread->thread = nullptr;
			thread->ostream = ostream;
			thread->newElementSet = New();
			tableThreadList.push_back( thread );
		}

		if( multiThreaded )
		{
			if( ostream )
				*ostream << "Kicking off " << tableThreadList.size() << " thread(s)!\n";

			for( std::list< CaylayTableThread* >::iterator iter = tableThreadList.begin(); iter != tableThreadList.end(); iter++ )
			{
				CaylayTableThread* thread = *iter;
				thread->thread = new std::thread( &ElementSet::CaylayTableThread::Generate, thread );
			}
		}
		else
		{
			for( std::list< CaylayTableThread* >::iterator iter = tableThreadList.begin(); iter != tableThreadList.end(); iter++ )
			{
				CaylayTableThread* thread = *iter;
				thread->Generate();
			}
		}

		ElementSetList elementSetList;

		while( tableThreadList.size() > 0 )
		{
			std::list< CaylayTableThread* >::iterator iter = tableThreadList.begin();
			CaylayTableThread* thread = *iter;
			if( multiThreaded )
				thread->thread->join();
			elementSetList.push_back( thread->newElementSet );
			delete thread->thread;
			delete thread;
			tableThreadList.erase( iter );

			if( ostream && multiThreaded )
				*ostream << "Thread joined.  " << tableThreadList.size() << " thread(s) remaining.\n";
		}

		std::list< UnionThread* > unionThreadList;
		ElementSetList::iterator setIter = elementSetList.begin();
		while( setIter != elementSetList.end() )
		{
			ElementSetList::iterator nextIter = setIter;
			nextIter++;

			UnionThread* thread = new UnionThread();
			thread->setTarget = *setIter;
			thread->setSource = ( nextIter == elementSetList.end() ) ? nullptr : *nextIter;
			thread->thread = nullptr;
			if( multiThreaded && thread->setSource )
				thread->thread = new std::thread( &UnionThread::Unionize, thread );
			unionThreadList.push_back( thread );

			setIter = nextIter;
			setIter++;
		}

		while( unionThreadList.size() > 1 )
		{
			if( !multiThreaded )
			{
				for( std::list< UnionThread* >::iterator iter = unionThreadList.begin(); iter != unionThreadList.end(); iter++ )
				{
					UnionThread* thread = *iter;
					if( thread->setSource )
					{
						thread->Unionize();
						delete thread->setSource;
						thread->setSource = nullptr;
					}
				}
			}

			bool combined = false;
			do
			{
				combined = false;

				for( std::list< UnionThread* >::iterator iterA = unionThreadList.begin(); iterA != unionThreadList.end(); iterA++ )
				{
					UnionThread* threadA = *iterA;
					if( threadA->setSource )
						continue;

					for( std::list< UnionThread* >::iterator iterB = iterA; iterB != unionThreadList.end(); iterB++ )
					{
						UnionThread* threadB = *iterB;
						if( threadA == threadB || threadB->setSource )
							continue;

						if( multiThreaded )
						{
							// This should be near instantaneous, because the threads have no more work to do.
							threadA->thread->join();
							threadB->thread->join();
							delete threadA->thread;
							delete threadB->thread;
						}

						UnionThread* thread = new UnionThread();
						thread->setTarget = threadA->setTarget;
						thread->setSource = threadB->setTarget;
						thread->thread = nullptr;
						unionThreadList.push_back( thread );

						if( multiThreaded )
							thread->thread = new std::thread( &UnionThread::Unionize, thread );

						unionThreadList.erase( iterA );
						unionThreadList.erase( iterB );

						delete threadA;
						delete threadB;

						combined = true;
						break;
					}

					if( combined )
						break;
				}
			}
			while( combined );
		}

		ElementSet* setUnion = nullptr;

		if( unionThreadList.size() > 1 )
		{
			UnionThread* thread = *unionThreadList.begin();
			if( multiThreaded )
			{
				thread->thread->join();
				delete thread->thread;
			}
			
			setUnion = thread->setTarget;
			delete thread;
			unionThreadList.clear();
		}

		if( setUnion )
		{
			// This is probably where we'll be spending most of our time.  :(
			AbsorbSet( *setUnion, &data->caylayTableHeaderArray );
			delete setUnion;
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
		}
	}

	return true;
}

void ElementSet::ChopUpBlockList( CaylayBlockList& blockList, uint maxThreadCount, uint minBlockSize )
{
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
		
		if( block.Width() > block.Height() )
		{
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
		
			blockList.push_back( leftBlock );
			blockList.push_back( rightBlock );
		}
		else
		{
			CaylayBlock topBlock, bottomBlock;

			uint halfHeight = block.Height() / 2;

			topBlock.minRow = block.minRow;
			topBlock.maxRow = block.minRow + halfHeight;
			topBlock.minCol = block.minCol;
			topBlock.maxCol = block.maxCol;

			bottomBlock.minRow = topBlock.maxRow + 1;
			bottomBlock.maxRow = block.maxRow;
			bottomBlock.minCol = block.minCol;
			bottomBlock.maxCol = block.maxCol;

			blockList.push_back( topBlock );
			blockList.push_back( bottomBlock );
		}

		blockList.erase( foundIter );
	}
}

void ElementSet::CaylayTableThread::Generate( void )
{
	for( uint i = block.minRow; i <= block.maxRow; i++ )
	{
		const Element* elementA = data->caylayTableHeaderArray[i];

		for( uint j = block.minCol; j <= block.maxCol; j++ )
		{
			const Element* elementB = data->caylayTableHeaderArray[j];

			Element* product = set->Multiply( elementA, elementB );

			if( !set->IsMember( product ) || !newElementSet->AddNewMember( product ) )
				delete product;
		}
	}
}

void ElementSet::UnionThread::Unionize( void )
{
	if( setSource )
	{
		setTarget->AbsorbSet( *setSource );
		delete setSource;
		setSource = nullptr;
	}
}

void ElementSet::AbsorbSet( ElementSet& set, ElementArray* elementArray /*= nullptr*/ )
{
	while( set.Cardinality() > 0 )
	{
		ElementList::iterator iter = set.elementList.begin();
		Element* element = *iter;
		set.elementList.erase( iter );

		if( !AddNewMember( element ) )
			delete element;
		else if( elementArray )
			elementArray->push_back( element );
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