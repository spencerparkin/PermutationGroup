// StabilizerChain.cpp

#include "StabilizerChain.h"
#include <memory>

//------------------------------------------------------------------------------------------
//                                   StabilizerChainGroup
//------------------------------------------------------------------------------------------

StabilizerChainGroup::StabilizerChainGroup( void )
{
	subGroup = nullptr;
}

StabilizerChainGroup::~StabilizerChainGroup( void )
{
	delete subGroup;
}

bool StabilizerChainGroup::Generate( uint stablePoint, uint maxStablePoint, WordCompressor* wordCompressor /*= nullptr*/, std::ostream* ostream /*= nullptr*/ )
{
	// TODO: Use the orbit-stabilizer theorem and Schreier's lemma.
	//       Use the basic orbit-algorithm.  Make sure all transversals contain the identity element.
	//       See: http://www.math.colostate.edu/~hulpke/talks/decomp.pdf
	//       In that PDF, we find that we need only iterate over the generators to find all orbits.
	//       This works, because every element is a product of the generators, and we keep track of
	//       every point we reach in a list that we continually run through all generators.  We need
	//       to track the chain of steps we take from point to point to point so that we can get the
	//       transversal element as a single permutation that takes us from the original point to the
	//       final point.  Keep a parent pointer with each orbit point so that we can construct this.

	return false;
}

bool StabilizerChainGroup::Factor( const PermutationElement& permElement, PermutationElement& invPermElement ) const
{
	return false;
}

void StabilizerChainGroup::Print( std::ostream& ostream ) const
{
	//...
}

// StabilizerChain.cpp