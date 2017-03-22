// StabilizerChain.h

#pragma once

#include "Permutation.h"

class StabilizerChain
{
public:

	StabilizerChain( void );
	virtual ~StabilizerChain( void );

	bool Generate( const PermutationSet& generatorSet );

	class Group
	{
	public:

		Group( void );
		virtual ~Group( void );

		bool Extend( const Permutation& generator );
		bool IsMember( const Permutation& permutation ) const;

		uint stabilizerPoint;
		PermutationSet generatorSet;
		PermutationSet transversalSet;
		Group* subGroup;
	};

	Group* group;
};

// StabilizerChain.h