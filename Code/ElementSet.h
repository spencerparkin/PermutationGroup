// ElementSet.h

#pragma once

#include "Word.h"
#include "Permutation.h"
#include <list>
#include <ostream>
#include <map>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>

typedef unsigned int uint;

//------------------------------------------------------------------------------------------
//                                         Element
//------------------------------------------------------------------------------------------

class ElementSet;

class Element
{
public:

	Element( void );
	virtual ~Element( void );

	virtual Element* MultiplyOnRight( const Element* element ) const { return nullptr; }
	virtual Element* MultiplyOnLeft( const Element* element ) const { return nullptr; }
	virtual bool IsEqualTo( const Element* element ) const { return false; }
	virtual bool IsInverseOf( const Element* element ) const { return false; }
	virtual Element* Invert( void ) const { return nullptr; }
	virtual Element* Clone( void ) const = 0;
	virtual std::string Name( void ) const = 0;
	virtual uint Order( void ) const = 0;
	virtual Word* GetWord( void ) { return nullptr; }
	virtual void Print( std::ostream& ostream ) const = 0;
	virtual std::size_t CalcHash( void ) const { return 0; }
};

typedef std::list< Element* > ElementList;
typedef std::vector< Element* > ElementArray;

//------------------------------------------------------------------------------------------
//                                        ElementKey
//------------------------------------------------------------------------------------------

class ElementKey
{
public:

	ElementKey( void );
	ElementKey( const ElementSet* set, const Element* element );
	ElementKey( const ElementKey& key );
	~ElementKey( void );

	bool operator==( const ElementKey& key ) const;
	void operator=( const ElementKey& key );

	std::size_t CalcHash( void ) const;

	const ElementSet* set;
	const Element* element;
};

typedef std::unordered_map< ElementKey, uint > ElementOffsetMap;

namespace std
{
	template<>
	struct hash< ElementKey >
	{
		std::size_t operator()( const ElementKey& key ) const
		{
			return key.CalcHash();
		}
	};
}

//------------------------------------------------------------------------------------------
//									       ElementSet
//------------------------------------------------------------------------------------------

// This is a set of group elements.  The set itself is not necessarily a group.
class ElementSet
{
public:

	ElementSet( void );
	virtual ~ElementSet( void );
	
	void Print( std::ostream& ostream ) const;

	virtual ElementSet* New( void ) const = 0;
	virtual ElementSet* Clone( void ) const;
	virtual Element* Identity( void ) const = 0;
	virtual Element* Multiply( const Element* elementA, const Element* elementB ) const;
	virtual Element* Invert( const Element* element ) const;
	virtual bool AreEqual( const Element* elementA, const Element* elementB ) const;
	virtual bool AreInverses( const Element* elementA, const Element* elementB ) const;

	uint Cardinality( void ) const;
	bool GenerateGroup( const ElementSet& generatorSet, std::ostream* ostream = nullptr, bool multiThreaded = true );
	void Clear( void );

	ElementArray elementArray;
	ElementOffsetMap elementOffsetMap;
	bool elementsHaveUniqueRepresentation;

	bool IsMember( const Element* element, uint* offset = nullptr );
	bool AddNewMember( Element* element, uint* offset = nullptr );

	struct CayleyColumn
	{
		Element* permuterElement;
		ElementArray columnArray;

		uint Grow( ElementSet* set );
	};

	uint RandomInteger( uint min, uint max );
};

typedef std::list< ElementSet* > ElementSetList;

//------------------------------------------------------------------------------------------
//                                  PermutationElement
//------------------------------------------------------------------------------------------

class PermutationElement : public Element
{
public:

	PermutationElement( void );
	virtual ~PermutationElement( void );

	virtual std::string Name( void ) const override;
	virtual Element* Clone( void ) const override;
	virtual uint Order( void ) const override;
	virtual void Print( std::ostream& ostream ) const override;
	virtual Word* GetWord( void ) override { return &word; }
	virtual std::size_t CalcHash( void ) const override { return permutation.CalcHash(); }

	Word word;
	Permutation permutation;
};

//------------------------------------------------------------------------------------------
//                                     PermutationSet
//------------------------------------------------------------------------------------------

class PermutationSet : public ElementSet
{
public:

	PermutationSet( void );
	virtual ~PermutationSet( void );

	virtual ElementSet* New( void ) const override;
	virtual Element* Identity( void ) const override;
	virtual Element* Multiply( const Element* elementA, const Element* elementB ) const override;
	virtual Element* Invert( const Element* element ) const override;
	virtual bool AreEqual( const Element* elementA, const Element* elementB ) const override;
	virtual bool AreInverses( const Element* elementA, const Element* elementB ) const override;
};

//------------------------------------------------------------------------------------------
//                                 CosetRepresentativeSet
//------------------------------------------------------------------------------------------

class CosetRepresentativeSet : public PermutationSet
{
public:

	CosetRepresentativeSet( void );
	virtual ~CosetRepresentativeSet( void );

	virtual ElementSet* New( void ) const override;
	virtual bool AreEqual( const Element* elementA, const Element* elementB ) const override;
	virtual bool AreInverses( const Element* elementA, const Element* elementB ) const override;

	virtual bool IsInDivsorGroup( const Permutation& permutation ) const { return false; }

	enum CosetType
	{
		LEFT_COSET,
		RIGHT_COSET,
	};

	CosetType cosetType;
};

//------------------------------------------------------------------------------------------
//                           StabilizerCosetRepresentativeSet
//------------------------------------------------------------------------------------------

// Stabilizer subgroups are not generally normal subgroups, but we can still
// generate transversals of the set of left or right cosets of such groups.
class StabilizerCosetRepresentativeSet : public CosetRepresentativeSet
{
public:

	StabilizerCosetRepresentativeSet( void );
	virtual ~StabilizerCosetRepresentativeSet( void );

	virtual bool IsInDivsorGroup( const Permutation& permutation ) const override;

	NaturalNumberSet stableSet;
};

// ElementSet.h