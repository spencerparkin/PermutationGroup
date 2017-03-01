// ElementSet.cpp

#include "ElementSet.h"

//------------------------------------------------------------------------------------------
//									         Element
//------------------------------------------------------------------------------------------

Element::Element( void )
{
	collection = nullptr;
}

Element::Element( const Element& element )
{
	collection = nullptr;
	element.permutation.GetCopy( permutation );
	element.word.GetCopy( word );
}

Element::~Element( void )
{
}

void Element::Identity( void )
{
	word.termList.clear();
	permutation.map->clear();
}

bool Element::operator==( const Element& element ) const
{
	if( collection )
		return collection->AreEqual( *this, element );

	return permutation.IsEqualTo( element.permutation );
}

void Element::operator=( const Element& element )
{
	element.word.GetCopy( word );
	element.permutation.GetCopy( permutation );
}

bool Element::Multiply( const Element& elementA, const Element& elementB )
{
	word.Multiply( elementA.word, elementB.word );
	permutation.Multiply( elementA.permutation, elementB.permutation );
	return true;
}

bool Element::MultiplyOnRight( const Element& element )
{
	word.MultiplyOnRight( element.word );
	permutation.MultiplyOnRight( element.permutation );
	return true;
}

bool Element::MultiplyOnLeft( const Element& element )
{
	word.MultiplyOnLeft( element.word );
	permutation.MultiplyOnleft( element.permutation );
	return true;
}

bool Element::SetInverse( const Element& element )
{
	return element.GetInverse( *this );
}

bool Element::GetInverse( Element& element ) const
{
	word.GetInverse( element.word );
	return permutation.GetInverse( element.permutation );
}

std::size_t Element::CalcHash( void ) const
{
	if( collection )
		return collection->CalcHash( *this );

	return permutation.CalcHash();
}

void Element::Print( std::ostream& ostream ) const
{
	word.Print( ostream );
	ostream << " = ";
	permutation.Print( ostream );
	ostream << "\n";
}

//------------------------------------------------------------------------------------------
//									   ElementCollection
//------------------------------------------------------------------------------------------

ElementCollection::ElementCollection( void )
{
}

/*virtual*/ ElementCollection::~ElementCollection( void )
{
}

bool ElementCollection::AddElement( Element& element )
{
	ElementSet::iterator iter;
	if( IsMember( element, &iter ) )
		return false;
	element.collection = this;
	elementSet.insert( element );
	return true;
}

bool ElementCollection::RemoveElement( Element& element )
{
	ElementSet::iterator iter;
	if( !IsMember( element, &iter ) )
		return false;
	element.collection = nullptr;
	elementSet.erase( iter );
	return true;
}

void ElementCollection::RemoveAllElements( void )
{
	elementSet.clear();
}

bool ElementCollection::IsMember( Element& element, ElementSet::iterator* foundIter /*= nullptr*/ )
{
	ElementSet::iterator iter = elementSet.find( element );
	if( iter == elementSet.end() )
		return false;
	if( foundIter )
		*foundIter = iter;
	return true;
}

/*virtual*/ bool ElementCollection::AreEqual( const Element& elementA, const Element& elementB )
{
	return elementA.permutation.IsEqualTo( elementB.permutation );
}

/*virtual*/ std::size_t ElementCollection::CalcHash( const Element& element ) const
{
	return element.permutation.CalcHash();
}

bool ElementCollection::GenerateGroup( std::ostream* ostream /*= nullptr*/ )
{
	ElementSet elementQueue;
	while( elementSet.size() > 0 )
	{
		ElementSet::iterator iter = elementSet.begin();
		const Element& element = *iter;
		if( !element.permutation.IsValid() )
			return false;
		elementQueue.insert( element );
		elementSet.erase( iter );
	}

	Element identity;
	elementSet.insert( identity );

	while( elementQueue.size() > 0 )
	{
		ElementSet::iterator queueIter = elementQueue.begin();
		Element newElement = *queueIter;
		elementQueue.erase( queueIter );

		// TODO: Despite this conditional, can I prove the correctness of this algorithm?
		if( !IsMember( newElement ) )
		{
			for( ElementSet::const_iterator iter = elementSet.cbegin(); iter != elementSet.cend(); iter++ )
			{
				const Element& element = *iter;

				for( int i = 0; i < 2; i++ )
				{
					Element product;

					if( i == 0 )
						product.Multiply( newElement, element );
					else
						product.Multiply( element, newElement );

					bool foundInSet = IsMember( product );
					bool foundInQueue = ( elementQueue.find( product ) == elementQueue.end() ) ? false : true;

					if( !( foundInSet || foundInQueue ) )
						elementQueue.insert( product );
				}
			}

			elementSet.insert( newElement );

			if( ostream )
				*ostream << "SetSize: " << elementSet.size() << ", QueueSize: " << elementQueue.size() << "\n";
		}
	}

	return true;
}

// Of course, if we're a group, then this is the order of the group.
uint ElementCollection::Cardinality( void ) const
{
	return ( uint )elementSet.size();
}

void ElementCollection::Print( std::ostream& ostream ) const
{
	ostream << "Cardinality: " << Cardinality() << "\n";

	for( ElementSet::const_iterator iter = elementSet.cbegin(); iter != elementSet.cend(); iter++ )
	{
		const Element& element = *iter;
		element.Print( ostream );
	}
}

//------------------------------------------------------------------------------------------
//									     CosetCollection
//------------------------------------------------------------------------------------------

CosetCollection::CosetCollection( void )
{
}

/*virtual*/ CosetCollection::~CosetCollection( void )
{
}

bool CosetCollection::AreInSameCoset( const Permutation& permutationA, const Permutation& permutationB ) const
{
	Permutation invPermutationA;
	permutationA.GetInverse( invPermutationA );

	Permutation product;
	product.Multiply( invPermutationA, permutationB );

	return IsInQuotientGroup( product );
}

bool CosetCollection::IsInQuotientGroup( const Permutation& permutation ) const
{
	NaturalNumberSet permUnstableSet;
	permutation.GetUnstableSet( permUnstableSet );

	return permUnstableSet.IsSubsetOf( unstableSet );
}

/*virtual*/ bool CosetCollection::AreEqual( const Element& elementA, const Element& elementB )
{
	return AreInSameCoset( elementA.permutation, elementB.permutation );
}

/*virtual*/ std::size_t CosetCollection::CalcHash( const Element& element ) const
{
	// Unfortunately, I'm not sure how to get around doing this linear search.
	// What it means is that we're essentially defeating the purpose of using a hash table.
	// What we would have to do to fix this is make sure that elements that would fall
	// under the same coset always hash to the same value.  I'm not sure how to do that.
	const Element* cosetRepresentative = FindCosetRepresentative( element );
	if( cosetRepresentative )
		return cosetRepresentative->CalcHash();

	return element.CalcHash();
}

const Element* CosetCollection::FindCosetRepresentative( const Element& element ) const
{
	for( ElementSet::const_iterator iter = elementSet.cbegin(); iter != elementSet.cend(); iter++ )
	{
		const Element& existingElement = *iter;
		if( AreInSameCoset( existingElement.permutation, element.permutation ) )
			return &existingElement;
	}

	return nullptr;
}

// ElementSet.cpp