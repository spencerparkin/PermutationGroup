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
bool StabilizerChain::Generate( const PermutationSet& generatorSet, const UintArray& baseArray )
{
	this->baseArray.clear();
	for( uint i = 0; i < baseArray.size(); i++ )
		this->baseArray.push_back( baseArray[i] );

	delete group;
	group = new Group( this, 0 );

	for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
	{
		const Permutation& generator = *iter;
		if( !group->Extend( generator ) )
			return false;
	}

	Group* subGroup = group;
	while( subGroup )
	{
		delete subGroup->rootNode;
		subGroup->rootNode = nullptr;
		subGroup = subGroup->subGroup;
	}

	return true;
}

StabilizerChain::Group::Group( StabilizerChain* stabChain, uint stabilizerOffset )
{
	this->stabChain = stabChain;
	this->stabilizerOffset = stabilizerOffset;
	subGroup = nullptr;
	rootNode = nullptr;
}

/*virtual*/ StabilizerChain::Group::~Group( void )
{
	delete subGroup;
	delete rootNode;
}

bool StabilizerChain::Group::Extend( const Permutation& generator )
{
	if( IsMember( generator ) )
		return true;

	generatorSet.insert( generator );

	PermutationSet singletonSet;
	singletonSet.insert( generator );

	bool fresh = false;
	if( !rootNode )
	{
		// The root of the orbit tree must be the identity to satisfy a requirement of Schreier's lemma.
		Permutation identity;
		transversalSet.insert( identity );
		PermutationSet::iterator iter = transversalSet.find( identity );
		rootNode = new OrbitNode( &( *iter ) );
		fresh = true;
	}

	if( !rootNode->Grow( this, generatorSet, singletonSet, fresh ) )
		return false;

	for( PermutationSet::iterator genIter = generatorSet.begin(); genIter != generatorSet.end(); genIter++ )
	{
		const Permutation& generator = *genIter;

		for( PermutationSet::iterator transIter = transversalSet.begin(); transIter != transversalSet.end(); transIter++ )
		{
			const Permutation& cosetRepresentative = *transIter;

			Permutation product;
			product.Multiply( cosetRepresentative, generator );

			PermutationSet::iterator iter = FindCoset( product );
			if( iter == transversalSet.end() )
				return false;		// Something went wrong with our math!

			Permutation invCosetRepresentative;
			invCosetRepresentative.SetInverse( *iter );

			Permutation schreierGenerator;
			schreierGenerator.Multiply( product, invCosetRepresentative );

			if( !schreierGenerator.IsIdentity() )
			{
				if( stabilizerOffset >= stabChain->baseArray.size() )
					return false;

				if( !subGroup )
					subGroup = new Group( stabChain, stabilizerOffset + 1 );

				if( !subGroup->Extend( schreierGenerator ) )
					return false;
			}
		}
	}

	return true;
}

PermutationSet::iterator StabilizerChain::Group::FindCoset( const Permutation& permutation )
{
	uint stabilizerPoint = stabChain->baseArray[ stabilizerOffset ];

	Permutation invPermutation;
	invPermutation.SetInverse( permutation );

	PermutationSet::iterator iter = transversalSet.begin();
	while( iter != transversalSet.end() )
	{
		const Permutation& cosetRepresentative = *iter;

		Permutation product;
		product.Multiply( cosetRepresentative, invPermutation );

		uint point = product.Evaluate( stabilizerPoint );
		if( point == stabilizerPoint )
			break;

		iter++;
	}

	return iter;
}

// Assuming that the stabilizer chain rooted as this node is valid, tell
// us if the given permutation element is a member of this group.
bool StabilizerChain::Group::IsMember( const Permutation& permutation ) const
{
	Permutation invPermutation;
	return FactorInverse( permutation, invPermutation );
}

bool StabilizerChain::Group::FactorInverse( const Permutation& permutation, Permutation& invPermutation ) const
{
	if( !subGroup )
		return true;

	uint stabilizerPoint = stabChain->baseArray[ stabilizerOffset ];
	if( permutation.Evaluate( stabilizerPoint ) == stabilizerPoint )
		return subGroup->FactorInverse( permutation, invPermutation );

	PermutationSet::iterator iter = const_cast< Group* >( this )->FindCoset( permutation );
	if( iter == transversalSet.end() )
		return false;

	const Permutation& cosetRepresentative = *iter;

	Permutation invCosetRepresentative;
	cosetRepresentative.GetInverse( invCosetRepresentative );

	Permutation product;
	product.Multiply( permutation, invCosetRepresentative );

	invPermutation.MultiplyOnRight( invCosetRepresentative );

	return FactorInverse( product, invPermutation );
}

StabilizerChain::OrbitNode::OrbitNode( const Permutation* cosetRepresentative )
{
	this->cosetRepresentative = cosetRepresentative;
}

StabilizerChain::OrbitNode::~OrbitNode( void )
{
	for( uint i = 0; i < adjacentNodeArray.size(); i++ )
		delete adjacentNodeArray[i];
}

bool StabilizerChain::OrbitNode::Grow( Group* group, const PermutationSet& generatorSet, const PermutationSet& singletonSet, bool fresh )
{
	uint stabilizerPoint = group->stabChain->baseArray[ group->stabilizerOffset ];

	const PermutationSet* permutationSet = &generatorSet;
	if( !fresh )
		permutationSet = &singletonSet;

	uint nonFreshSize = adjacentNodeArray.size();

	for( PermutationSet::const_iterator iter = permutationSet->cbegin(); iter != permutationSet->cend(); iter++ )
	{
		const Permutation& generator = *iter;

		Permutation permutation;
		permutation.Multiply( *cosetRepresentative, generator );

		uint point = permutation.Evaluate( stabilizerPoint );

		if( !group->orbitSet.IsMember( point ) )
		{
			group->orbitSet.AddMember( point );

			group->transversalSet.insert( permutation );
			PermutationSet::iterator iter = group->transversalSet.find( permutation );

			OrbitNode* orbitNode = new OrbitNode( &( *iter ) );
			adjacentNodeArray.push_back( orbitNode );
			if( !orbitNode->Grow( group, generatorSet, singletonSet, true ) )
				return false;
		}
	}

	for( uint i = 0; i < nonFreshSize; i++ )
		adjacentNodeArray[i]->Grow( group, generatorSet, singletonSet, false );

	return true;
}

// StabilizerChain.cpp