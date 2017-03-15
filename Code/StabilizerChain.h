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

	//void Print( std::ostream& ostream ) const;

	bool Generate( std::ostream* ostream = nullptr );

	//bool Factor();

	//bool Load();
	//bool Save();

	PermutationSet generatorSet;
	PermutationSet transversalSet;

	StabilizerChainGroup* subGroup;

	uint stabilizerPoint;

	PermutationSet::iterator FindCoset( const Permutation& permutation );
};

// StabilizerChain.h