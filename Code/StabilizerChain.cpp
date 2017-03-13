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
	//       This is a BFS of the Caylay graph along generator edges that advances the frontier
	//       only if a new point in the orbit is encountered.  It can terminate when the queue
	//       is empty and be sure it found the entire orbit.

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