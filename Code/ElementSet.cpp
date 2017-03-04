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

	for( uint i = 0; i < elementArray.size(); i++ )
	{
		const Element* member = elementArray[i];
		set->elementArray.push_back( member->Clone() );
	}

	return set;
}

bool ElementSet::GenerateGroup( const ElementSet& generatorSet, std::ostream* ostream /*= nullptr*/, bool multiThreaded /*= true*/ )
{
	Clear();
	for( uint i = 0; i < generatorSet.elementArray.size(); i++ )
		AddNewMember( generatorSet.elementArray[i]->Clone() );

	Element* permutationElement = elementArray[0];

	ElementArray columnArray;

	uint setLocation = 0;

	while( true )
	{
		// If we break this into two passes, one pass if fast, the other could be farmed out to threads.
		while( setLocation < elementArray.size() )
		{
			Element* element = elementArray[ setLocation ];
			Element* product = Multiply( element, permutationElement );
			
			uint offset;
			if( !IsMember( product, &offset ) )
				elementArray.push_back( product );
			else
			{
				delete product;
				product = elementArray[ offset ];
			}

			columnArray.push_back( product );

			setLocation++;
		}

		// At this point, we either have the entire group,
		// or we need to search for a new element...randomly?

		// If we find a new element g, and S is our current column,
		// then I believe we can unconditionally add gS, g^2S, ..., g^{|g|-1}S
		// to our elementArray.  Now begin again at the top of this loop to build
		// our column.
	}

	return true;
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

bool ElementSet::IsMember( const Element* element, uint* offset /*= nullptr*/ )
{
	// This linear search is the main innefficiency in my design.
	// If elements have a unique representation, then we can use a hash table to get better performance.
	// In the case of factor groups, however, I do not know how to do better than this linear search.
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

	return false;
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