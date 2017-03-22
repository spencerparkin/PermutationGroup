// StabilizerChain.cpp

#include "StabilizerChain.h"

StabilizerChain::StabilizerChain( void )
{
	group = nullptr;
}

/*virtual*/ StabilizerChain::~StabilizerChain( void )
{
	delete group;
}

// This is an attempt to impliment the Schreier-Sims algorithm.
bool StabilizerChain::Generate( const PermutationSet& generatorSet )
{
	delete group;
	group = new Group();

	for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
	{
		const Permutation& generator = *iter;
		if( !group->Extend( generator ) )
			return false;
	}

	return true;
}

StabilizerChain::Group::Group( void )
{
	subGroup = nullptr;
	stabilizerPoint = -1;
}

/*virtual*/ StabilizerChain::Group::~Group( void )
{
	delete subGroup;
}

bool StabilizerChain::Group::Extend( const Permutation& generator )
{
	// TODO: Add the given generator to our group if it is not already a member of the group.
	//       If it is a member, we can ignore it.  If it is not, then we add the generator,
	//       find new orbits/transversal elements, then use the Schreier lemma to make new
	//       generators for our subgroup and recursively extend the subgroup.  We want to make
	//       sure that when we're done, we've modified the chain in a way that it is always
	//       a complete chain for the main group of the chain.
	//
	//       There are some complications that will arrise.  As soon as we add a generator to
	//       a group, making it bigger, its subgroup's transversal set may become entirely invalid.
	//       We have to keep in mind what sub-chain of our chain is valid/complete at every given point.

	return true;
}

// Assuming that the stabilizer chain rooted as this node is valid, tell
// us if the given permutation element is a member of this group.
bool StabilizerChain::Group::IsMember( const Permutation& permutation ) const
{
	return true;
}

// StabilizerChain.cpp