// StabilizerChain.h

#pragma once

#include "Permutation.h"
#include "NaturalNumberSet.h"
#include <vector>
#include <iostream>
#include <string>
#include "rapidjson/document.h"

typedef std::vector< uint > UintArray;

class PermutationStreamCreator;
class PermutationStream;

// One idea that would certainly reduce factorization sizes is to use a stabilizer tree, instead of a chain.
// This would come at high memory cost, unless the tree wasn't as full as it could be.  In any case, the sifting
// process could make a decision about which branch of the tree to go down in order to minimize collisions with
// transversal elements.  It might not be hard to modify the Schreier-Sims algorithm to handle a base-tree and
// generate a stabilizer tree from it.
class StabilizerChain
{
public:

	class Group;

	StabilizerChain( void );
	virtual ~StabilizerChain( void );

	bool Generate( const PermutationSet& generatorSet, const UintArray& baseArray );
	void Print( std::ostream& ostream ) const;
	bool LoadFromJsonString( const std::string& jsonString );
	bool SaveToJsonString( std::string& jsonString ) const;
	uint Depth( void ) const;
	Group* GetSubGroupAtDepth( uint depth );
	const Group* GetSubGroupAtDepth( uint depth ) const;
	StabilizerChain* Clone( void ) const;

	class OrbitNode;
	class Group;

	typedef std::vector< OrbitNode* > OrbitNodeArray;

	class OrbitNode
	{
	public:

		OrbitNode( const Permutation* cosetRepresentative );
		~OrbitNode( void );

		bool Grow( Group* group, const PermutationSet& generatorSet, const PermutationSet& singletonSet, bool fresh, OrbitNodeArray& newOrbitArray );

		const Permutation* cosetRepresentative;
		OrbitNodeArray adjacentNodeArray;
	};

	struct Stats
	{
		Stats( void );

		uint totalUnnamedTransversalCount;
		uint totalUnnamedGeneratorCount;
		UintArray unnamedTransversalCountArray;
		UintArray unnamedGeneratorCountArray;

		void Reset( void );
		void Print( std::ostream& ostream ) const;
	};

	class Group
	{
	public:

		Group( StabilizerChain* stabChain, Group* superGroup, uint stabilizerOffset );
		virtual ~Group( void );

		bool Extend( const Permutation& generator, bool* extended = nullptr );
		bool Eliminate( void );		// If successful, the caller owns the group memory and should delete it if they don't want it.
		bool IsMember( const Permutation& permutation ) const;
		bool FactorInverse( const Permutation& permutation, Permutation& invPermutation ) const;
		bool FactorInverseWithTrembling( const Permutation& permutation, Permutation& invPermutation, const PermutationSet& trembleSet, const CompressInfo& compressInfo ) const;
		PermutationSet::iterator FindCoset( const Permutation& permutation );
		const NaturalNumberSet& GetSubgroupStabilizerPointSet( void ) const;
		void Print( std::ostream& ostream ) const;
		bool StabilizesPoint( uint point ) const;
		void NameGenerators( void );
		bool MakeCompressInfo( CompressInfo& compressInfo );
		bool OptimizeNameWithPermutation( Permutation& permutation, const CompressInfo& compressInfo );
		void AccumulateStats( Stats& stats ) const;
		bool LoadRecursive( /*const*/ rapidjson::Value& chainGroupValue );
		bool SaveRecursive( rapidjson::Value& chainGroupValue, rapidjson::Document::AllocatorType& allocator ) const;
		unsigned long long Order( void ) const;
		bool IsSubGroupOf( const Group& group ) const;

		NaturalNumberSet orbitSet;
		OrbitNode* rootNode;
		uint stabilizerOffset;
		PermutationSet generatorSet;
		PermutationSet transversalSet;
		Group* subGroup;
		Group* superGroup;
		StabilizerChain* stabChain;
	};

	typedef bool ( *OptimizeNamesCallback )( const Stats*, bool, double, void* );
	bool OptimizeNames( PermutationStream& permutationStream, const CompressInfo& compressInfo, OptimizeNamesCallback callback, void* callback_data = nullptr );
	bool IsCompletelyWorded( void ) const;
	bool FindUnwordedCosetRepresentative( Group*& subGroup, PermutationSet::iterator& iter );
	bool TryToCompletePartiallyWordedChain( PermutationStream& permutationStream, const CompressInfo& compressInfo, OptimizeNamesCallback callback, void* callback_data = nullptr );

	Group* group;
	NaturalNumberSetArray baseArray;
	std::ostream* logStream;
};

// StabilizerChain.h