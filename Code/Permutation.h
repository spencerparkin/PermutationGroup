// Permutation.h

#pragma once

#include <set>
#include <list>
#include <map>
#include <vector>
#include <string>
#include <unordered_set>
#include "rapidjson/document.h"
#include "NaturalNumberSet.h"

class Permutation;

typedef std::list< Permutation > PermutationList;
typedef std::map< std::string, Permutation > PermutationMap;
typedef std::vector< Permutation > PermutationArray;
typedef std::vector< const Permutation* > PermutationConstPtrArray;
typedef std::unordered_set< Permutation > PermutationSet;
typedef std::set< Permutation > OrderedPermutationSet;

typedef unsigned int uint;

struct Element
{
	std::string name;
	int exponent;
};

typedef std::list< Element > ElementList;
typedef std::map< std::string, uint > OrderMap;
typedef std::map< std::string, bool > CommuteMap;

class CompressInfo
{
public:

	bool ElementsCommute( const Element& elementA, const Element& elementB ) const;
	uint ElementOrder( const Element& element ) const;

	mutable OrderMap orderMap;
	mutable CommuteMap commuteMap;
	PermutationMap permutationMap;
};

//------------------------------------------------------------------------------------------
//                                        Permutation
//------------------------------------------------------------------------------------------

#define MAX_MAP_SIZE			15

// A permutation is a bijective map defined on a subset of the natural numbers.
// Here, multiplication is function composition.  For convenience, we let every
// permutation be defined on the entire set of natural number, though only a
// subset of the naturals can actually be mapped.  Anything outside that subset
// is considered preserved by the permutation.
class Permutation
{
public:

	Permutation( void );
	Permutation( const Permutation& permutation );
	virtual ~Permutation( void );

	uint Evaluate( uint input ) const;
	bool Stabilizes( const NaturalNumberSet& set ) const;
	bool Define( uint input, uint output );
	bool DefineCycle( uint a, uint b );
	bool DefineCycle( uint a, uint b, uint c );
	bool DefineCycle( uint a, uint b, uint c, uint d );
	bool DefineCycle( uint a, uint b, uint c, uint d, uint e );
	bool DefineCycle( uint a, uint b, uint c, uint d, uint e, uint f );
	bool DefineCycleArray( const uint* cycleArray, uint cycleArraySize );
	void DefineIdentity( void );
	bool IsValid( void ) const;
	bool IsEven( void ) const;
	bool IsOdd( void ) const;
	bool IsIdentity( void ) const;
	bool IsEqualTo( const Permutation& permutation ) const;
	bool CommutesWith( const Permutation& permutation ) const;
	uint Order( void ) const;
	uint CycleOrder( void ) const;
	void SetCopy( const Permutation& permutation, bool copyWord = true );
	void GetCopy( Permutation& permutation, bool copyWord = true ) const;
	bool SetInverse( const Permutation& permutation );
	bool GetInverse( Permutation& permutation ) const;
	void Multiply( const Permutation& permutationA, const Permutation& permutationB );
	void MultiplyOnRight( const Permutation& permutation );
	void MultiplyOnLeft( const Permutation& permutation );
	bool Factor( PermutationList& permutationList ) const;
	void GetUnstableSet( NaturalNumberSet& unstableSet ) const;
	void GetStableSet( NaturalNumberSet& stableSet ) const;
	std::size_t CalcHash( void ) const;
	void Print( std::ostream& ostream, bool isCycle = false ) const;
	bool operator==( const Permutation& permutation ) const;
	void operator=( const Permutation& permutation );
	void SetName( const std::string& name );
	std::string GetName( void ) const;
	bool GetToJsonValue( rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator ) const;
	bool SetFromJsonValue( /*const*/ rapidjson::Value& value );
	bool CompressWord( const CompressInfo& compressInfo );
	static bool LexigraphicCompare( const Permutation& permLeft, const Permutation& permRight );
	std::string MakeKeyForLexigraphicCompare( void ) const;
	static bool LoadPermutationSet( PermutationSet& permutationSet, /*const*/ rapidjson::Value& arrayValue );
	static bool SavePermutationSet( const PermutationSet& permutationSet, rapidjson::Value& arrayValue, rapidjson::Document::AllocatorType& allocator );
	bool LoadFromJsonString( const std::string& jsonString );
	bool SaveToJsonString( std::string& jsonString ) const;

	ElementList* word;
	uint map[ MAX_MAP_SIZE ];
};

namespace std
{
	template<>
	struct hash< Permutation >
	{
		std::size_t operator()( const Permutation& permutation ) const
		{
			return permutation.CalcHash();
		}
	};

	template<>
	struct less< Permutation >
	{
		bool operator()( const Permutation& permLeft, const Permutation& permRight ) const
		{
			return Permutation::LexigraphicCompare( permLeft, permRight );
		}
	};
}

// Permutation.h