// StabilizerChain.cpp

#include "StabilizerChain.h"
#include <time.h>

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
				subGroup = new Group( stabChain, stabilizerOffset + 1 );

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
	uint stabilizerPoint = GetSubgroupStabilizerPoint();
	if( permutation.Evaluate( stabilizerPoint ) == stabilizerPoint )
	{
		if( !subGroup )
			return true;

		return subGroup->FactorInverse( permutation, invPermutation );
	}

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

void StabilizerChain::Group::NameGenerators( void )
{
	char name = 'a';

	PermutationSet::iterator iter = generatorSet.begin();
	while( iter != generatorSet.end() )
	{
		PermutationSet::iterator nextIter = iter;
		nextIter++;

		Permutation permutation = *iter;
		if( !permutation.word )
		{
			Element element;
			element.name = name++;
			element.exponent = 1;

			permutation.word = new ElementList;
			permutation.word->push_back( element );

			generatorSet.erase( iter );
			generatorSet.insert( permutation );
		}

		iter = nextIter;
	}
}

bool StabilizerChain::Group::MakeCompressInfo( CompressInfo& compressInfo )
{
	compressInfo.permutationMap.clear();
	compressInfo.commuteMap.clear();
	compressInfo.orderMap.clear();

	for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
	{
		const Permutation& generator = *iter;

		if( !generator.word || generator.word->size() != 1 )
			return false;

		const Element& element = *generator.word->begin();

		compressInfo.permutationMap.insert( std::pair< std::string, Permutation >( element.name, generator ) );
	}

	return true;
}

// The key idea here is taken from Minkwitz, although I'm sure that it's use here is not exactly what he had in mind.
bool StabilizerChain::Group::OptimizeNames( const CompressInfo& compressInfo )
{
	std::ostream* logStream = stabChain->logStream;

	Group* group = this;
	while( group )
	{
		// The identity element must be used as a coset representative, so give it a name now.
		for( PermutationSet::iterator iter = group->transversalSet.begin(); iter != group->transversalSet.end(); iter++ )
		{
			const Permutation& permutation = *iter;
			if( permutation.IsIdentity() && !permutation.word )
			{
				group->transversalSet.erase( iter );
				Permutation identity;
				identity.word = new ElementList;
				group->transversalSet.insert( identity );
				break;
			}
		}

		group = group->subGroup;
	}

	PermutationSet processedSet;
	PermutationSet permutationQueue;
	Permutation identity;
	identity.word = new ElementList;
	permutationQueue.insert( identity );

	clock_t lastOptimizationTime = clock();
	double timeOutSec = 60.0;
	uint allUnnamedCount = -1;

	while( permutationQueue.size() > 0 )
	{
		// TODO: We need to impliment a container for permutations that
		//       sorts by word-length and then pull the shortest words
		//       off the queue each iteration.  An AVL tree can do this,
		//       but maybe I can use a standard container for this.
		PermutationSet::iterator iter = permutationQueue.begin();
		Permutation permutation = *iter;
		permutationQueue.erase( iter );

		if( logStream )
		{
			//*logStream << "Trying...\n";
			//permutation.Print( *ostream );
		}

		// TODO: I'm finding "cc^{-1}" and "cc" in words that were supposedly compressed.  :(
		permutation.CompressWord( compressInfo );

		if( OptimizeNameWithPermutation( permutation ) )
		{
			lastOptimizationTime = clock();

			allUnnamedCount = CountAllUnnamedRepresentatives();
			if( logStream )
				*logStream << "Unnamed count: " << allUnnamedCount << "\n";
		}

		processedSet.insert( permutation );
		
		for( iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
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

		clock_t currentTime = clock();
		double elapsedTimeSec = double( currentTime - lastOptimizationTime ) / double( CLOCKS_PER_SEC );

		if( elapsedTimeSec > timeOutSec && CountUnnamedRepresentatives() == 0 )
			break;

		// When this count goes to zero, we can bail at any time, but by staying in longer,
		// we can hopefully optimize more representatives.  Ideally we could get all of them
		// to be under a certain word length.
		if( elapsedTimeSec > timeOutSec && allUnnamedCount == 0 )
			break;
	}

	stabChain->Print( *logStream );

	processedSet.clear();
	permutationQueue.clear();

	/*
	uint unnamedCount = CountAllUnnamedRepresentatives();
	if( unnamedCount > 0 )
	{
		group = this;
		while( group && group->CountUnnamedRepresentatives() == 0 )
			group = group->subGroup;

		if( !group )
			return false;

		// This won't work, because the generators for this sub-group
		// would have to have factorizations in terms of the original generators.
		// If we carried them through using Schreier's lemma, they would be huge!
		return group->OptimizeNames( compressInfo );
	}*/

	return true;
}

bool StabilizerChain::Group::OptimizeNameWithPermutation( const Permutation& permutation )
{
	if( !permutation.word )
		return false;

	uint stabilizerPoint = GetSubgroupStabilizerPoint();
	if( permutation.Evaluate( stabilizerPoint ) == stabilizerPoint )
	{
		if( !subGroup )
			return false;

		return subGroup->OptimizeNameWithPermutation( permutation );
	}

	PermutationSet::iterator iter = FindCoset( permutation );
	if( iter == transversalSet.end() )
		return false;

	std::ostream* logStream = stabChain->logStream;

	const Permutation& cosetRepresentative = *iter;

	if( ( cosetRepresentative.word && permutation.word->size() < cosetRepresentative.word->size() ) || !cosetRepresentative.word )
	{
		if( logStream )
		{
			*logStream << "Replacing...\n";
			cosetRepresentative.Print( *logStream );
			*logStream << "...with...\n";
			permutation.Print( *logStream );
		}

		transversalSet.erase( iter );
		transversalSet.insert( permutation );
		return true;
	}

	Permutation invPermutation;
	permutation.GetInverse( invPermutation );

	Permutation product;
	product.Multiply( cosetRepresentative, invPermutation );

	return OptimizeNameWithPermutation( product );
}

uint StabilizerChain::Group::CountUnnamedRepresentatives( void ) const
{
	uint count = 0;

	for( PermutationSet::const_iterator iter = transversalSet.cbegin(); iter != transversalSet.cend(); iter++ )
	{
		const Permutation& permutation = *iter;
		if( !permutation.word )
			count++;
	}

	return count;
}

uint StabilizerChain::Group::CountAllUnnamedRepresentatives( void ) const
{
	uint count = CountUnnamedRepresentatives();

	if( subGroup )
		count += subGroup->CountAllUnnamedRepresentatives();

	return count;
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