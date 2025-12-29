// NaturalNumberSet.h

#pragma once

#include <set>
#include <iostream>
#include <vector>
#include "rapidjson/document.h"

typedef unsigned int uint;

class NaturalNumberSet;

typedef std::vector< NaturalNumberSet > NaturalNumberSetArray;

//------------------------------------------------------------------------------------------
//                                    NaturalNumberSet
//------------------------------------------------------------------------------------------

class NaturalNumberSet
{
public:

	NaturalNumberSet( void );
	NaturalNumberSet( const NaturalNumberSet& set );
	virtual ~NaturalNumberSet( void );

	bool IsEmpty( void ) const;
	bool IsMember( uint x ) const;
	void AddMember( uint x );
	void RemoveMember( uint x );
	void RemoveAllMembers( void );
	void Print( std::ostream& ostream ) const;

	void GetToJsonValue( rapidjson::Value& setValue, rapidjson::Document::AllocatorType& allocator ) const;
	void SetFromJsonValue( /*const*/ rapidjson::Value& setValue );

	uint Cardinality( void ) const;

	void Copy( const NaturalNumberSet& set, bool clear = true );
	void Union( const NaturalNumberSet& setA, const NaturalNumberSet& setB );
	void Intersect( const NaturalNumberSet& setA, const NaturalNumberSet& setB );

	bool IsSubsetOf( const NaturalNumberSet& givenSet ) const;
	bool IsSupsetOf( const NaturalNumberSet& givenSet ) const;

	void GenerateDivisorsOf( uint x );

	uint CalcLCM( void ) const;
	uint CalcGCD( void ) const;

	uint Max( void ) const;
	uint Min( void ) const;

	static uint Lcm( uint a, uint b );
	static uint Gcd( uint a, uint b );

	typedef std::set< uint > UintSet;
	UintSet set;
};

// NaturalNumberSet.h
