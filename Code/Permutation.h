// Permutation.h

#pragma once

#include <set>
#include <list>
#include <map>
#include <vector>
#include <string>
#include <unordered_set>
#include <rapidjson/document.h>
#include "NaturalNumberSet.h"

class Permutation;

typedef std::list< Permutation > PermutationList;
typedef std::map< std::string, Permutation > PermutationMap;
typedef std::vector< Permutation > PermutationArray;

typedef unsigned int uint;

struct Element
{
	std::string name;
	int exponent;
};

typedef std::list< Element > ElementList;

//------------------------------------------------------------------------------------------
//                                        Permutation
//------------------------------------------------------------------------------------------

#define MAX_MAP_SIZE			40

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
	bool Define( uint input, uint output );
	void DefineIdentity( void );
	bool IsValid( void ) const;
	bool IsEven( void ) const;
	bool IsOdd( void ) const;
	bool IsIdentity( void ) const;
	bool IsEqualTo( const Permutation& permutation ) const;
	bool CommutesWith( const Permutation& permutation ) const;
	uint Order( void ) const;
	uint CycleOrder( void ) const;
	void SetCopy( const Permutation& permutation );
	void GetCopy( Permutation& permutation ) const;
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
	bool CompressWord( const PermutationMap& permutationMap );

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
}

typedef std::unordered_set< Permutation > PermutationSet;

// Permutation.h