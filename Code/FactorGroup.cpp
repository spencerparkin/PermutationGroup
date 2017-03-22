// FactorGroup.cpp

#include "FactorGroup.h"

FactorGroup::FactorGroup( void )
{
}

/*virtual*/ FactorGroup::~FactorGroup( void )
{
}

// This is lame.
void FactorGroup::Generate( const PermutationSet& generatorSet, std::ostream* ostream /*= nullptr*/ )
{
	PermutationSet processedSet;
	PermutationSet permutationQueue;
	Permutation identity;
	permutationQueue.insert( identity );

	uint consecutiveHitCount = 0;
	uint consecutiveHitCountMax = 100;

	// We're trying to get the factor group generated without needing
	// to actually enumerate every member of the group.  I'm not sure
	// how to do that, so we're estimating when we might be done.  Yes, it's lame.
	while( permutationQueue.size() > 0 && consecutiveHitCount < consecutiveHitCountMax )
	{
		PermutationSet::iterator iter = permutationQueue.begin();
		Permutation permutation = *iter;
		permutationQueue.erase( iter );

		PermutationSet::iterator foundIter = FindCoset( permutation );
		if( foundIter != cosetRepresentativeSet.end() )
			consecutiveHitCount++;
		else
		{
			consecutiveHitCount = 0;
			cosetRepresentativeSet.insert( permutation );
		}

		for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
		{
			const Permutation& generator = *iter;

			Permutation newPermutation;
			newPermutation.Multiply( permutation, generator );

			if( processedSet.find( newPermutation ) == processedSet.end() &&
				permutationQueue.find( newPermutation ) == permutationQueue.end() )
			{
				permutationQueue.insert( newPermutation );
			}
		}
	}
}

PermutationSet::iterator FactorGroup::FindCoset( const Permutation& cosetRepresentative )
{
	Permutation invCosetRepresentative;
	cosetRepresentative.GetInverse( invCosetRepresentative );

	PermutationSet::iterator iter = cosetRepresentativeSet.begin();
	while( iter != cosetRepresentativeSet.end() )
	{
		Permutation product;
		product.Multiply( ( *iter ), invCosetRepresentative );

		if( IsInDivsorGroup( product ) )
			break;

		iter++;
	}

	return iter;
}

/*virtual*/ bool FactorGroup::IsInDivsorGroup( const Permutation& permutation ) const
{
	return false;
}

// FactorGroup.cpp