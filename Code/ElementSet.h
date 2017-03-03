// ElementSet.h

#pragma once

#include "Word.h"
#include "Permutation.h"
#include <list>
#include <ostream>
#include <map>

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
	virtual Element* Invert( void ) const { return nullptr; }
	virtual Element* Clone( void ) const = 0;
	virtual std::string Name( void ) const = 0;
	virtual uint Order( void ) const = 0;
	virtual Word* GetWord( void ) { return nullptr; }
	virtual void Print( std::ostream& ostream ) const = 0;
};

typedef std::list< Element* > ElementList;

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

	uint Cardinality( void ) const;
	bool GenerateGroup( std::ostream* ostream = nullptr );
	void Clear( void );

	static void DeleteList( ElementList& elementList );

	ElementList elementList;

	bool IsMember( const Element* element, ElementList::iterator* foundIter = nullptr );
	bool AddNewMember( Element* element );
};

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

	bool IsInDivsorGroup( const Permutation& permutation ) const;

	NaturalNumberSet unstableSet;
};

// ElementSet.h