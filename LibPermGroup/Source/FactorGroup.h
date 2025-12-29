// FactorGroup.h

#pragma once

#include "Permutation.h"
#include <iostream>

class FactorGroup
{
public:

	FactorGroup( void );
	virtual ~FactorGroup( void );

	void Generate( const PermutationSet& generatorSet, std::ostream* ostream = nullptr );

	virtual bool IsInDivsorGroup( const Permutation& permutation ) const;

	PermutationSet::iterator FindCoset( const Permutation& cosetRepresentative );

	PermutationSet cosetRepresentativeSet;
};

// FactorGroup.h
