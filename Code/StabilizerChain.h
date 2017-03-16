// StabilizerChain.h

#pragma once

#include "Permutation.h"

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
		PRINT_FLAG_GENERATORS			= 0x00000001,
		PRINT_FLAG_REPRESENTATIVES		= 0x00000002,
	};

	void Print( std::ostream& ostream, uint flags = PRINT_FLAG_REPRESENTATIVES ) const;

	bool Generate( uint* pointArray, uint pointArraySize, uint pointArrayOffset, std::ostream* ostream = nullptr );
	bool Optimize( std::ostream* ostream = nullptr );

	//bool Factor();

	//bool Load();
	//bool Save();

	PermutationSet generatorSet;
	PermutationSet transversalSet;

	StabilizerChainGroup* subGroup;

	// This is the point stabilized by the subgroup.
	uint stabilizerPoint;

	bool CalculateSchreierGenerators( PermutationSet& schreierGeneratorSet );
	void EnqueueNewPermutations( const Permutation& permutation, PermutationSet& permutationQueue, PermutationSet* processedSet = nullptr );
	PermutationSet::iterator FindCoset( const Permutation& permutation );
	bool OptimizeWithPermutation( const Permutation& permutation, std::ostream* ostream = nullptr );
};

// StabilizerChain.h