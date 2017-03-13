// StabilizerChain.h

#pragma once

#include "ElementSet.h"

//------------------------------------------------------------------------------------------
//                                   StabilizerChainGroup
//------------------------------------------------------------------------------------------

class WordCompressor;

class StabilizerChainGroup
{
public:

	StabilizerChainGroup( void );
	~StabilizerChainGroup( void );

	void Print( std::ostream& ostream ) const;
	bool Generate( uint stablePoint, uint maxStablePoint, WordCompressor* wordCompressor = nullptr, std::ostream* ostream = nullptr );
	bool Factor( const PermutationElement& permElement, PermutationElement& invPermElement ) const;

	//bool Load();
	//bool Save();

	PermutationSet generatorSet;
	PermutationSet transversalSet;

	StabilizerChainGroup* subGroup;
};

// StabilizerChain.h