// StabilizerChain.h

#pragma once

#include "Permutation.h"
#include "NaturalNumberSet.h"
#include <vector>
#include <iostream>
#include <string>
#include <rapidjson/document.h>

typedef std::vector< uint > UintArray;

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

	class Group
	{
	public:

		Group( StabilizerChain* stabChain, Group* superGroup, uint stabilizerOffset );
		virtual ~Group( void );

		bool Extend( const Permutation& generator );
		Group* Eliminate( void );
		bool IsMember( const Permutation& permutation ) const;
		bool FactorInverse( const Permutation& permutation, Permutation& invPermutation ) const;
		bool FactorInverseWithTrembling( const Permutation& permutation, Permutation& invPermutation, const PermutationSet& trembleSet, const CompressInfo& compressInfo ) const;
		PermutationSet::iterator FindCoset( const Permutation& permutation );
		uint GetSubgroupStabilizerPoint( void ) const;
		void Print( std::ostream& ostream ) const;
		bool StabilizesPoint( uint point ) const;
		void NameGenerators( void );
		bool MakeCompressInfo( CompressInfo& compressInfo );
		bool OptimizeNames( const CompressInfo& compressInfo, double timeOutSec = 60.0 );
		bool OptimizeNameWithPermutation( Permutation& permutation, const CompressInfo& compressInfo );
		uint CountUnnamedRepresentatives( void ) const;
		uint CountAllUnnamedRepresentatives( void ) const;
		bool LoadRecursive( /*const*/ rapidjson::Value& chainGroupValue );
		bool SaveRecursive( rapidjson::Value& chainGroupValue, rapidjson::Document::AllocatorType& allocator ) const;
		long long Order( void ) const;

		NaturalNumberSet orbitSet;
		OrbitNode* rootNode;
		uint stabilizerOffset;
		PermutationSet generatorSet;
		PermutationSet transversalSet;
		Group* subGroup;
		Group* superGroup;
		StabilizerChain* stabChain;
	};

	Group* group;
	UintArray baseArray;
	std::ostream* logStream;
};

// StabilizerChain.h