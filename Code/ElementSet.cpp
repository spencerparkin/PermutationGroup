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
//                                        ElementKey
//------------------------------------------------------------------------------------------

ElementKey::ElementKey( void )
{
	set = nullptr;
	element = nullptr;
}

ElementKey::ElementKey( const ElementSet* set, const Element* element )
{
	this->set = set;
	this->element = element;
}

ElementKey::ElementKey( const ElementKey& key )
{
	set = key.set;
	element = key.element;
}

ElementKey::~ElementKey( void )
{
}

bool ElementKey::operator==( const ElementKey& key ) const
{
	if( set != key.set )
		return false;

	return set->AreEqual( element, key.element );
}

void ElementKey::operator=( const ElementKey& key )
{
	set = key.set;
	element = key.element;
}

std::size_t ElementKey::CalcHash( void ) const
{
	return element->CalcHash();
}

//------------------------------------------------------------------------------------------
//									       ElementSet
//------------------------------------------------------------------------------------------

ElementSet::ElementSet( void )
{
	elementsHaveUniqueRepresentation = false;
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

/*virtual*/ bool ElementSet::AreInverses( const Element* elementA, const Element* elementB ) const
{
	return elementA->IsInverseOf( elementB );
}

/*virtual*/ ElementSet* ElementSet::Clone( void ) const
{
	ElementSet* set = New();

	set->elementsHaveUniqueRepresentation = elementsHaveUniqueRepresentation;

	for( uint i = 0; i < elementArray.size(); i++ )
	{
		const Element* member = elementArray[i];
		Element* memberClone = member->Clone();
		set->elementArray.push_back( memberClone );

		if( elementsHaveUniqueRepresentation )
			set->elementOffsetMap.insert( std::pair< ElementKey, uint >( ElementKey( set, memberClone ), uint( set->elementArray.size() ) ) );
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

// The idea here is to generate any kind of group, but I believe we can also use this routine to generate transversals.
// On second thought, I'm not sure this is guarenteed to get us every element we would need in any transversal.
// Of course, if the subgroup is normal, then it should generate the factor group.
// As of this writing, unfortunately, there is no clear stopping condition for the algorithm.
bool ElementSet::Generate( const ElementSet& generatorSet, std::ostream* ostream /*= nullptr*/, bool multiThreaded /*= true*/ )
{
	Clear();
	for( uint i = 0; i < generatorSet.elementArray.size(); i++ )
		AddNewMember( generatorSet.elementArray[i]->Clone() );

	if( generatorSet.Cardinality() == 0 )
		return true;

	std::srand(0);

	typedef std::list< CayleyColumn* > CayleyColumnList;
	CayleyColumnList cayleyColumnList;

	// Arbitrarily pick an element to use as our first cayley column.
	CayleyColumn* cayleyColumn = new CayleyColumn();
	cayleyColumn->permuterElement = elementArray[0];
	cayleyColumnList.push_back( cayleyColumn );

	uint stagnationCount = 0;

	while( true )
	{
		uint oldOrder = ( uint )elementArray.size();

		// Grow all cayley columns until no one column grows.
		uint growCount = 0;
		uint maxGrowth;
		while( true )
		{	
			maxGrowth = 0;
			for( CayleyColumnList::iterator iter = cayleyColumnList.begin(); iter != cayleyColumnList.end(); iter++ )
			{
				CayleyColumn* cayleyColumn = *iter;
				uint growth = cayleyColumn->Grow( this );
				if( growth > maxGrowth )
					maxGrowth = growth;
			}
			if( maxGrowth > 0 )
				growCount++;
			else
				break;
		}

		uint newOrder = ( uint )elementArray.size();

		if( newOrder == oldOrder )
			stagnationCount++;
		else
			stagnationCount = 0;

		if( ostream )
		{
			*ostream << "Size: " << elementArray.size() << "\n";
			*ostream << "Columns: " << cayleyColumnList.size() << "\n";
		}

		// After growing all columns, if we grew as many as there are known elements of the group,
		// then we have generated the entire group.  This is unlikely for large groups, but may happen
		// if we're trying to generate a small group.
		if( cayleyColumnList.size() == elementArray.size() )
			break;

		// Terminate now as an approximation of having generated the group if we were stagnet long enough.
		// In many cases, we have generated the entire group, and we can add more columns to our heart's
		// content and it will never generate any new elements.
		// TODO: What we really need in place of this is a proven stopping condition.  Checking for closure
		//       is as hard as generating the entire Cayley table, which we just don't have time to do.
		//       More research is clearly indicated.  I was able to conclude that if the column, as a permutation,
		//       has the same order as the size of the column, then we have the group, but this only happens
		//       for cyclic groups according to Cayley's theorem.
		if( stagnationCount > 15 )		// This is a hacky guess for now.
			break;

		// Pick an element at random to start a new column.
		while( true )
		{
			uint i = RandomInteger( 0, uint( elementArray.size() ) - 1 );
			CayleyColumnList::iterator iter = cayleyColumnList.begin();
			while( iter != cayleyColumnList.end() )
			{
				CayleyColumn* cayleyColumn = *iter;
				if( cayleyColumn->permuterElement == elementArray[i] )
					break;
				iter++;
			}
			if( iter == cayleyColumnList.end() )
			{
				CayleyColumn* cayleyColumn = new CayleyColumn();
				cayleyColumn->permuterElement = elementArray[i];
				cayleyColumnList.push_back( cayleyColumn );
				break;
			}
		}
	}

	while( cayleyColumnList.size() > 0 )
	{
		CayleyColumnList::iterator iter = cayleyColumnList.begin();
		CayleyColumn* cayleyColumn = *iter;
		delete cayleyColumn;
		cayleyColumnList.erase( iter );
	}

	return true;
}

uint ElementSet::CayleyColumn::Grow( ElementSet* set )
{
	uint growth = 0;

	while( columnArray.size() < set->elementArray.size() )
	{
		Element* element = set->elementArray[ columnArray.size() ];
		Element* product = set->Multiply( element, permuterElement );

		uint offset;
		if( !set->AddNewMember( product, &offset ) )
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
	return ( uint )elementArray.size();
}

void ElementSet::Clear( void )
{
	for( uint i = 0; i < elementArray.size(); i++ )
	{
		Element* member = elementArray[i];
		delete member;
	}

	elementArray.clear();
	elementOffsetMap.clear();
}

void ElementSet::Print( std::ostream& ostream ) const
{
	for( uint i = 0; i < elementArray.size(); i++ )
	{
		const Element* member = elementArray[i];
		member->Print( ostream );
	}
}

bool ElementSet::IsMember( const Element* element, uint* offset /*= nullptr*/ )
{
	if( elementsHaveUniqueRepresentation )
	{
		ElementKey key( this, element );
		ElementOffsetMap::iterator iter = elementOffsetMap.find( key );
		if( iter == elementOffsetMap.end() )
			return false;
		
		if( offset )
			*offset = iter->second;

		return true;
	}
	else
	{
		// This linear search is the main innefficiency in the program.
		// If elements have a unique representation, then we can use a hash table to get better performance.
		// In the case of factor groups, however, I do not know how to do better than a linear search.
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

	return false;
}

bool ElementSet::AddNewMember( Element* element, uint* offset /*= nullptr*/ )
{
	if( IsMember( element, offset ) )
		return false;

	if( elementsHaveUniqueRepresentation )
	{
		ElementKey key( this, element );
		elementOffsetMap.insert( std::pair< ElementKey, uint >( key, ( uint )elementArray.size() ) );
	}

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

/*virtual*/ bool PermutationSet::AreInverses( const Element* elementA, const Element* elementB ) const
{
	const PermutationElement* permElementA = dynamic_cast< const PermutationElement* >( elementA );
	const PermutationElement* permElementB = dynamic_cast< const PermutationElement* >( elementB );

	if( !( permElementA && permElementB ) )
		return false;

	Permutation invPermutationA;
	permElementA->permutation.GetInverse( invPermutationA );

	return invPermutationA.IsEqualTo( permElementB->permutation );
}

//------------------------------------------------------------------------------------------
//                                 CosetRepresentativeSet
//------------------------------------------------------------------------------------------

CosetRepresentativeSet::CosetRepresentativeSet( void )
{
	cosetType = LEFT_COSET;
}

/*virtual*/ CosetRepresentativeSet::~CosetRepresentativeSet( void )
{
}

/*virtual*/ ElementSet* CosetRepresentativeSet::New( void ) const
{
	return new CosetRepresentativeSet();
}

/*virtual*/ bool CosetRepresentativeSet::AreEqual( const Element* elementA, const Element* elementB ) const
{
	const PermutationElement* permElementA = dynamic_cast< const PermutationElement* >( elementA );
	const PermutationElement* permElementB = dynamic_cast< const PermutationElement* >( elementB );

	if( !( permElementA && permElementB ) )
		return false;

	Permutation product;

	switch( cosetType )
	{
		case LEFT_COSET:
		{
			Permutation invPermA;
			invPermA.SetInverse( permElementA->permutation );
			product.Multiply( invPermA, permElementB->permutation );
			break;
		}
		case RIGHT_COSET:
		{
			Permutation invPermB;
			invPermB.SetInverse( permElementB->permutation );
			product.Multiply( permElementA->permutation, invPermB );
			break;
		}
		default:
		{
			return false;
		}
	}

	return IsInDivsorGroup( product );
}

/*virtual*/ bool CosetRepresentativeSet::AreInverses( const Element* elementA, const Element* elementB ) const
{
	const PermutationElement* permElementA = dynamic_cast< const PermutationElement* >( elementA );
	const PermutationElement* permElementB = dynamic_cast< const PermutationElement* >( elementB );

	if( !( permElementA && permElementB ) )
		return false;

	Permutation product;
	product.Multiply( permElementA->permutation, permElementB->permutation );

	return IsInDivsorGroup( product );
}

//------------------------------------------------------------------------------------------
//                           StabilizerCosetRepresentativeSet
//------------------------------------------------------------------------------------------

StabilizerCosetRepresentativeSet::StabilizerCosetRepresentativeSet( void )
{
}

/*virtual*/ StabilizerCosetRepresentativeSet::~StabilizerCosetRepresentativeSet( void )
{
}

/*virtual*/ bool StabilizerCosetRepresentativeSet::IsInDivsorGroup( const Permutation& permutation ) const
{
	NaturalNumberSet permStableSet;
	permutation.GetStableSet( permStableSet );

	return stableSet.IsSubsetOf( permStableSet );
}

// ElementSet.cpp