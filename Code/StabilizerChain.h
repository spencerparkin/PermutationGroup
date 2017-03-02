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

	bool Generate( const NaturalNumberSet& domainSet, WordCompressor* wordCompressor = nullptr, std::ostream* ostream = nullptr );
	bool Factor( const PermutationElement& permElement, PermutationElement& invPermElement ) const;

	PermutationSet generatorSet;
	CosetRepresentativeSet factorGroup;

	StabilizerChainGroup* subGroup;
};

// StabilizerChain.h