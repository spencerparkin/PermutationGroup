// StabilizerChain.h

#pragma once

#include "Permutation.h"
#include <rapidjson/document.h>

//------------------------------------------------------------------------------------------
//                                   StabilizerChainGroup
//------------------------------------------------------------------------------------------

class StabilizerChainGroup
{
public:

	StabilizerChainGroup( void );
	~StabilizerChainGroup( void );

	enum
	{
		FLAG_GENERATORS				= 0x00000001,
		FLAG_REPRESENTATIVES		= 0x00000002,
	};

	void Print( std::ostream& ostream, uint flags = FLAG_REPRESENTATIVES ) const;

	bool Generate( uint* pointArray, uint pointArraySize, uint pointArrayOffset, bool generateWords, std::ostream* ostream = nullptr );
	void NameGenerators( CompressInfo& compressInfo );
	bool Optimize( const CompressInfo& compressInfo, std::ostream* ostream = nullptr );

	// The given inverse permutation should be initialized to the identity before this call is made.
	bool FactorInverse( const Permutation& permutation, Permutation& invPermutation ) const;

	bool LoadFromJsonString( const std::string& jsonString );
	bool SaveToJsonString( std::string& jsonString, uint flags = FLAG_REPRESENTATIVES ) const;

	bool LoadRecursive( /*const*/ rapidjson::Value& chainGroupValue );
	bool SaveRecursive( rapidjson::Value& chainGroupValue, rapidjson::Document::AllocatorType& allocator, uint flags ) const;

	PermutationSet generatorSet;
	PermutationSet transversalSet;

	StabilizerChainGroup* subGroup;

	// This is the point stabilized by the subgroup.
	uint stabilizerPoint;

	bool CalculateSchreierGenerators( PermutationSet& schreierGeneratorSet );
	void EnqueueNewPermutations( const Permutation& permutation, PermutationSet& permutationQueue, PermutationSet* processedSet = nullptr );
	PermutationSet::iterator FindCoset( const Permutation& permutation );
	bool OptimizeWithPermutation( const Permutation& permutation, std::ostream* ostream = nullptr );
	uint CountUnnamedRepresentatives( void ) const;
	uint CountAllUnnamedRepresentatives( void ) const;

	static bool LoadPermutationSet( PermutationSet& permutationSet, /*const*/ rapidjson::Value& arrayValue );
	static bool SavePermutationSet( const PermutationSet& permutationSet, rapidjson::Value& arrayValue, rapidjson::Document::AllocatorType& allocator );
};

// StabilizerChain.h