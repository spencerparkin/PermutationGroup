// ElementSet.h

#pragma once

#include "Word.h"
#include "Permutation.h"
#include <unordered_set>
#include <list>

//------------------------------------------------------------------------------------------
//                                         Element
//------------------------------------------------------------------------------------------

class ElementCollection;

class Element
{
public:

	Element( void );
	Element( const Element& element );
	~Element( void );

	bool operator==( const Element& element ) const;
	void operator=( const Element& element );

	bool Multiply( const Element& elementA, const Element& elementB );
	bool MultiplyOnRight( const Element& element );
	bool MultiplyOnLeft( const Element& element );
	bool SetInverse( const Element& element );
	bool GetInverse( Element& element ) const;
	std::size_t CalcHash( void ) const;

	ElementCollection* collection;
	Word word;
	Permutation permutation;
};

typedef std::list< Element > ElementList;

//------------------------------------------------------------------------------------------
//										  ElementSet
//------------------------------------------------------------------------------------------

namespace std
{
	template<>
	struct hash< Element >
	{
		std::size_t operator()( const Element& element ) const
		{
			return element.CalcHash();
		}
	};
}

typedef std::unordered_set< Element > ElementSet;

//------------------------------------------------------------------------------------------
//									   ElementCollection
//------------------------------------------------------------------------------------------

// An element collection is just an element set with the concept of equality abstracted away.
class ElementCollection
{
public:

	ElementCollection( void );
	virtual ~ElementCollection( void );

	bool AddElement( Element& element );
	bool RemoveElement( Element& element );
	void RemoveAllElements( void );
	bool GenerateGroup( void );
	
	virtual std::size_t CalcHash( const Element& element ) const;
	virtual bool AreEqual( const Element& elementA, const Element& elementB );

	ElementSet elementSet;

	bool IsMember( Element& element, ElementSet::iterator* foundIter = nullptr );
};

//------------------------------------------------------------------------------------------
//									     CosetCollection
//------------------------------------------------------------------------------------------

class CosetCollection : public ElementCollection
{
public:

	CosetCollection( void );
	virtual ~CosetCollection( void );

	virtual std::size_t CalcHash( const Element& element ) const override;
	virtual bool AreEqual( const Element& elementA, const Element& elementB ) override;

	const Element* FindCosetRepresentative( const Element& element ) const;

	bool IsInQuotientGroup( const Permutation& permutation ) const;
	bool AreInSameCoset( const Permutation& permutationA, const Permutation& permutationB ) const;

	NaturalNumberSet unstableSet;
};

// ElementSet.h