// ElementSet.h

#pragma once

#include "Word.h"
#include "Permutation.h"
#include <list>
#include <ostream>
#include <map>
#include <vector>
#include <unordered_set>
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

class ElementKey
{
public:

	ElementKey( void )
	{
		element = nullptr;
	}

	ElementKey( Element* element )
	{
		this->element = element;
	}

	ElementKey( const ElementKey& key )
	{
		element = key.element;
	}

	~ElementKey( void )
	{
	}

	bool operator==( const ElementKey& key ) const
	{
		return ( element == key.element ) ? true : false;
	}

	void operator=( const ElementKey& key )
	{
		element = key.element;
	}

	Element* element;
};

typedef std::unordered_set< ElementKey > ElementHashMap;
typedef std::vector< ElementHashMap* > ElementHashMapArray;

namespace std
{
	template<>
	struct hash< ElementKey >
	{
		std::size_t operator()( const ElementKey& key ) const
		{
			return key.element->CalcHash();
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

	uint Cardinality( void ) const;
	bool GenerateGroup( const ElementSet& generatorSet, std::ostream* ostream = nullptr, bool multiThreaded = true );
	void Clear( void );

	static void DeleteList( ElementList& elementList );

	ElementList elementList;

	bool IsMember( const Element* element, ElementList::iterator* foundIter = nullptr );
	bool AddNewMember( Element* element );

	struct CaylayBlock
	{
		uint minRow, maxRow;
		uint minCol, maxCol;

		uint Width( void ) const { return maxCol - minCol + 1; }
		uint Height( void ) const { return maxRow - minRow + 1; }
		uint Area( void ) const { return Width() * Height(); }
	};

	typedef std::list< CaylayBlock > CaylayBlockList;

	struct CaylayTableData
	{
		std::mutex caylayTableHeaderArrayMutex;	// This is used to protect write access to the header array.
		ElementArray caylayTableHeaderArray;	// This is a shared resource, but I don't think we need to lock for read access.
		ElementHashMapArray caylayColumnCheckArray;	// No lock is needed for this shared resource, because no blocks overlap vertically.
	};

	void ChopUpBlockList( CaylayBlockList& blockList, uint maxThreadCount, uint minBlockSize );

	struct Thread
	{
		void Generate( void );

		CaylayBlock block;
		CaylayTableData* data;
		ElementSet* set;
		std::thread* thread;
	};
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