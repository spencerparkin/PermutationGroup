// ElementSet.h

#pragma once

#include "Word.h"
#include "Permutation.h"
#include <list>
#include <ostream>

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

	virtual Element* Clone( void ) const = 0;
	virtual uint Order( void ) const = 0;
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
	
	virtual ElementSet* New( void ) const = 0;
	virtual ElementSet* Clone( void ) const;
	virtual Element* Identity( void ) const = 0;
	virtual Element* Multiply( const Element* elementA, const Element* elementB ) const = 0;
	virtual Element* Invert( const Element* element ) const = 0;
	virtual bool AreEqual( const Element* elementA, const Element* elementB ) const = 0;

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

	virtual Element* Clone( void ) const override;
	virtual uint Order( void ) const override;
	virtual void Print( std::ostream& ostream ) const override;

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

	NaturalNumberSet unstableSet;
};

// ElementSet.h