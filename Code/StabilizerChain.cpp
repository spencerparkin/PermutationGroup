// StabilizerChain.cpp

#include "StabilizerChain.h"

StabilizerChain::StabilizerChain( void )
{
	group = nullptr;
	logStream = nullptr;
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

	freeOffsetSet.RemoveAllMembers();
	for( uint i = 1; i < baseArray.size(); i++ )
		freeOffsetSet.AddMember(i);

	if( logStream )
		*logStream << "Generating stabilizer chain!!!\n";

	uint size = generatorSet.size();
	uint count = 0;

	for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
	{
		const Permutation& generator = *iter;

		if( logStream )
		{
			count++;
			*logStream << "===============================================\n";
			*logStream << "              NEW GENERATOR (" << count << "/" << size << ")\n";
			*logStream << "===============================================\n";
			generator.Print( *logStream );
			*logStream << "===============================================\n";
		}
		
		if( !group->Extend( generator ) )
			return false;

		if( logStream )
			Print( *logStream );
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

void StabilizerChain::Print( std::ostream& ostream ) const
{
	ostream << "===============================================\n";
	ostream << "Stabilizer chain begin {\n";
	if( !group )
		ostream << "Empty!\n";
	else
		group->Print( ostream );
	ostream << "} Stabilizer chain end.\n";
	ostream << "===============================================\n";
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

void StabilizerChain::Group::Print( std::ostream& ostream ) const
{
	ostream << "-----------------------------------------\n";
	ostream << "Group has sub-group stabilizing: " << GetSubgroupStabilizerPoint() << "\n";

	ostream << "Generator set:\n";

	for( PermutationSet::const_iterator iter = generatorSet.cbegin(); iter != generatorSet.cend(); iter++ )
		( *iter ).Print( ostream );

	ostream << "Transversal set:\n";

	for( PermutationSet::const_iterator iter = transversalSet.cbegin(); iter != transversalSet.cend(); iter++ )
		( *iter ).Print( ostream );

	if( subGroup )
		subGroup->Print( ostream );
}

uint StabilizerChain::Group::GetSubgroupStabilizerPoint( void ) const
{
	return stabChain->baseArray[ stabilizerOffset ];
}

bool StabilizerChain::Group::Extend( const Permutation& generator )
{
	// TODO: Is there a problem with the IsMember function?  I notcied that an element
	//       and its inverse ended up a group's generating set.  That shouldn't happen, right?
	if( IsMember( generator ) )
		return true;

	std::ostream* logStream = stabChain->logStream;

	if( logStream )
	{
		//*logStream << "Extending group with new generator.\n";
		//generator.Print( *logStream );
	}

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
		orbitSet.AddMember( GetSubgroupStabilizerPoint() );
		fresh = true;
	}

	struct Pair
	{
		const Permutation* cosetRepresentative;
		const Permutation* generator;
	};

	typedef std::list< Pair > PairList;
	PairList pairList;

	for( PermutationSet::iterator iter = transversalSet.begin(); iter != transversalSet.end(); iter++ )
	{
		Pair pair;
		pair.cosetRepresentative = &( *iter );
		pair.generator = &generator;
		pairList.push_back( pair );
	}

	OrbitNodeArray newOrbitArray;
	if( !rootNode->Grow( this, generatorSet, singletonSet, fresh, newOrbitArray ) )
		return false;

	for( uint i = 0; i < newOrbitArray.size(); i++ )
	{
		const Permutation* cosetRepresentative = newOrbitArray[i]->cosetRepresentative;
		for( PermutationSet::iterator genIter = generatorSet.begin(); genIter != generatorSet.end(); genIter++ )
		{
			Pair pair;
			pair.generator = &( *genIter );
			pair.cosetRepresentative = cosetRepresentative;
			pairList.push_back( pair );
		}
	}

	for( PairList::iterator pairIter = pairList.begin(); pairIter != pairList.end(); pairIter++ )
	{
		Pair& pair = *pairIter;
		const Permutation& cosetRepresentative = *pair.cosetRepresentative;
		const Permutation& generator = *pair.generator;

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
			{
				// If we don't choose the subgroup's stabilizer point like this,
				// we can end up with non-proper subgroups in the change, and I believe
				// this causes a major innefficiency.
				uint i;
				for( i = 0; i < stabChain->baseArray.size(); i++ )
					if( !StabilizesPoint( stabChain->baseArray[i] ) && stabChain->freeOffsetSet.IsMember(i) )
						break;

				if( i == stabChain->baseArray.size() )
					return false;

				stabChain->freeOffsetSet.RemoveMember(i);

				subGroup = new Group( stabChain, i );
			}

			if( !subGroup->Extend( schreierGenerator ) )
				return false;
		}
	}

	return true;
}

bool StabilizerChain::Group::StabilizesPoint( uint point ) const
{
	for( PermutationSet::const_iterator iter = generatorSet.cbegin(); iter != generatorSet.cend(); iter++ )
	{
		const Permutation& generator = *iter;
		if( point != generator.Evaluate( point ) )
			return false;
	}

	return true;
}

PermutationSet::iterator StabilizerChain::Group::FindCoset( const Permutation& permutation )
{
	uint stabilizerPoint = GetSubgroupStabilizerPoint();

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
	FactorInverse( permutation, invPermutation );

	Permutation product;
	product.Multiply( permutation, invPermutation );

	return product.IsIdentity();
}

bool StabilizerChain::Group::FactorInverse( const Permutation& permutation, Permutation& invPermutation ) const
{
	if( !subGroup )
		return true;

	uint stabilizerPoint = GetSubgroupStabilizerPoint();
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

bool StabilizerChain::OrbitNode::Grow( Group* group, const PermutationSet& generatorSet, const PermutationSet& singletonSet, bool fresh, OrbitNodeArray& newOrbitArray )
{
	uint stabilizerPoint = group->GetSubgroupStabilizerPoint();

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

			std::ostream* logStream = group->stabChain->logStream;
			if( logStream )
			{
				*logStream << "Found new orbit: " << point << "\n";
				permutation.Print( *logStream );
			}

			group->transversalSet.insert( permutation );
			PermutationSet::iterator iter = group->transversalSet.find( permutation );

			OrbitNode* orbitNode = new OrbitNode( &( *iter ) );
			newOrbitArray.push_back( orbitNode );
			adjacentNodeArray.push_back( orbitNode );
			if( !orbitNode->Grow( group, generatorSet, singletonSet, true, newOrbitArray ) )
				return false;
		}
	}

	for( uint i = 0; i < nonFreshSize; i++ )
		adjacentNodeArray[i]->Grow( group, generatorSet, singletonSet, false, newOrbitArray );

	return true;
}

// StabilizerChain.cpp