// StabilizerChain.cpp

#include "StabilizerChain.h"
#include "PermutationStream.h"
#include <time.h>
#include <rapidjson/writer.h>

StabilizerChain::StabilizerChain( void )
{
	group = nullptr;
	logStream = nullptr;
}

/*virtual*/ StabilizerChain::~StabilizerChain( void )
{
	delete group;
}

uint StabilizerChain::Depth( void ) const
{
	uint i = 0;

	Group* subGroup = group;
	while( subGroup )
	{
		i++;
		subGroup = subGroup->subGroup;
	}

	return i;
}

StabilizerChain::Group* StabilizerChain::GetSubGroupAtDepth( uint depth )
{
	Group* foundGroup = group;

	uint i = 0;
	while( i < depth && foundGroup )
	{
		foundGroup = foundGroup->subGroup;
		i++;
	}

	return foundGroup;
}

bool StabilizerChain::LoadFromJsonString( const std::string& jsonString )
{
	delete group;
	group = new Group( this, nullptr, 0 );

	rapidjson::Document doc;
	doc.Parse( jsonString.c_str() );
	if( !doc.IsObject() )
		return false;

	if( !doc.HasMember( "group" ) )
		return false;

	rapidjson::Value chainGroupValue = doc[ "group" ].GetObject();
	if( !group->LoadRecursive( chainGroupValue ) )
		return false;

	if( !doc.HasMember( "baseArray" ) )
		return false;

	baseArray.clear();
	rapidjson::Value baseArrayValue = doc[ "baseArray" ].GetArray();
	for( uint i = 0; i < baseArrayValue.Size(); i++ )
		baseArray.push_back( baseArrayValue[i].GetInt() );

	return true;
}

bool StabilizerChain::SaveToJsonString( std::string& jsonString ) const
{
	if( !group )
		return false;

	rapidjson::Document doc;
	doc.SetObject();

	rapidjson::Value chainGroupValue( rapidjson::kObjectType );
	if( !group->SaveRecursive( chainGroupValue, doc.GetAllocator() ) )
		return false;

	doc.AddMember( "group", chainGroupValue, doc.GetAllocator() );

	rapidjson::Value baseArrayValue( rapidjson::kArrayType );
	for( uint i = 0; i < baseArray.size(); i++ )
		baseArrayValue.PushBack( baseArray[i], doc.GetAllocator() );

	doc.AddMember( "baseArray", baseArrayValue, doc.GetAllocator() );

	rapidjson::StringBuffer buffer;
	rapidjson::Writer< rapidjson::StringBuffer > writer( buffer );
	if( !doc.Accept( writer ) )
		return false;

	jsonString = buffer.GetString();
	return true;
}

// This is an attempt to impliment the Schreier-Sims algorithm.
bool StabilizerChain::Generate( const PermutationSet& generatorSet, const UintArray& baseArray )
{
	this->baseArray.clear();
	for( uint i = 0; i < baseArray.size(); i++ )
		this->baseArray.push_back( baseArray[i] );

	delete group;
	group = new Group( this, nullptr, 0 );

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

StabilizerChain::Group::Group( StabilizerChain* stabChain, Group* superGroup, uint stabilizerOffset )
{
	this->stabChain = stabChain;
	this->stabilizerOffset = stabilizerOffset;
	subGroup = nullptr;
	this->superGroup = superGroup;
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

StabilizerChain::Group* StabilizerChain::Group::Eliminate( void )
{
	if( !subGroup )
		return nullptr;

	std::vector< const Permutation* > permutationArray;

	for( PermutationSet::iterator iter = subGroup->transversalSet.begin(); iter != subGroup->transversalSet.end(); iter++ )
	{
		const Permutation* permutation = &( *iter );
		permutationArray.push_back( permutation );
	}

	for( PermutationSet::iterator iter = transversalSet.begin(); iter != transversalSet.end(); iter++ )
	{
		const Permutation& permutation = *iter;
		if( permutation.IsIdentity() )
			continue;

		for( uint i = 0; i < permutationArray.size(); i++ )
		{
			Permutation product;
			product.Multiply( *permutationArray[i], permutation );

			// This should always increase the size of the set.
			subGroup->transversalSet.insert( product );
		}
	}

	if( !superGroup )
	{
		stabChain->group = subGroup;
		subGroup->superGroup = nullptr;
	}
	else
	{
		superGroup->subGroup = subGroup;
		subGroup->superGroup = superGroup;
	}

	subGroup = nullptr;
	return this;
}

long long StabilizerChain::Group::Order( void ) const
{
	long long subGroupIndex = transversalSet.size();
	long long subGroupOrder = 1;
	if( subGroup )
		subGroupOrder = subGroup->Order();
	long long groupOrder = subGroupIndex * subGroupOrder;
	return groupOrder;
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
				subGroup = new Group( stabChain, this, stabilizerOffset + 1 );

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

// This idea comes from a paper by Egner and Puschel.  It is a smarter way of utilizing
// a stabilizer chain to come up with a factorization for the given permutation.
bool StabilizerChain::Group::FactorInverseWithTrembling( const Permutation& permutation, Permutation& invPermutation, const PermutationSet& trembleSet, const CompressInfo& compressInfo ) const
{
	if( !FactorInverse( permutation, invPermutation ) )
		return false;

	bool optimizationMade;

	do
	{
		optimizationMade = false;

		for( PermutationSet::const_iterator iter = trembleSet.cbegin(); iter != trembleSet.cend(); iter++ )
		{
			const Permutation& trembler = *iter;
			if( !trembler.word )
				return false;

			Permutation product;
			product.Multiply( permutation, trembler );

			Permutation invProduct;
			invProduct.word = new ElementList;
			if( !FactorInverse( product, invProduct ) )
				return false;

			Permutation altInvPermutation;
			altInvPermutation.word = new ElementList;
			altInvPermutation.Multiply( trembler, invProduct );

			altInvPermutation.CompressWord( compressInfo );

			if( altInvPermutation.word->size() < invPermutation.word->size() )
			{
				optimizationMade = true;
				altInvPermutation.GetCopy( invPermutation );
				break;
			}
		}
	}
	while( optimizationMade );

	return true;
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

bool StabilizerChain::Group::LoadRecursive( /*const*/ rapidjson::Value& chainGroupValue )
{
	if( !chainGroupValue.HasMember( "stabilizerOffset" ) )
		return false;

	stabilizerOffset = chainGroupValue[ "stabilizerOffset" ].GetInt();

	if( chainGroupValue.HasMember( "generators" ) )
	{
		rapidjson::Value generatorsValue = chainGroupValue[ "generators" ].GetArray();
		if( !Permutation::LoadPermutationSet( generatorSet, generatorsValue ) )
			return false;
	}

	if( chainGroupValue.HasMember( "transversals" ) )
	{
		rapidjson::Value transversalsValue = chainGroupValue[ "transversals" ].GetArray();
		if( !Permutation::LoadPermutationSet( transversalSet, transversalsValue ) )
			return false;
	}

	if( chainGroupValue.HasMember( "subGroup" ) )
	{
		subGroup = new Group( stabChain, this, 0 );

		rapidjson::Value subGroupValue = chainGroupValue[ "subGroup" ].GetObject();
		if( !subGroup->LoadRecursive( subGroupValue ) )
			return false;
	}

	return true;
}

bool StabilizerChain::Group::SaveRecursive( rapidjson::Value& chainGroupValue, rapidjson::Document::AllocatorType& allocator ) const
{
	chainGroupValue.AddMember( "stabilizerOffset", stabilizerOffset, allocator );

	rapidjson::Value generatorsValue( rapidjson::kArrayType );
	if( !Permutation::SavePermutationSet( generatorSet, generatorsValue, allocator ) )
		return false;

	chainGroupValue.AddMember( "generators", generatorsValue, allocator );

	rapidjson::Value transversalsValue( rapidjson::kArrayType );
	if( !Permutation::SavePermutationSet( transversalSet, transversalsValue, allocator ) )
		return false;

	chainGroupValue.AddMember( "transversals", transversalsValue, allocator );

	if( subGroup )
	{
		rapidjson::Value subGroupValue( rapidjson::kObjectType );
		if( !subGroup->SaveRecursive( subGroupValue, allocator ) )
			return false;

		chainGroupValue.AddMember( "subGroup", subGroupValue, allocator );
	}

	return true;
}

// The idea here is taken from Minkwitz, but I believe he did not have to do this as a
// post-process of Schreier-Sims, because he already knew the order of the group.  So he
// actually generated the chain while he was coming up with short words for the transversal
// elements.  He knew he could stop when the product of the lengths of all transversal sets
// was equal the group order.  More ideas for filling in the transversal element words can
// be found in a paper by Egner and Puschel.
bool StabilizerChain::OptimizeNames( PermutationStabGroupStream& permutationStream, const CompressInfo& compressInfo, double timeOutSec /*= 60.0*/ )
{
	Group* subGroup = group;
	while( subGroup )
	{
		// The identity element must be used as a coset representative, so give it
		// a name now that is sure not to be replaced by anything else.
		for( PermutationSet::iterator iter = subGroup->transversalSet.begin(); iter != subGroup->transversalSet.end(); iter++ )
		{
			const Permutation& permutation = *iter;
			if( permutation.IsIdentity() && !permutation.word )
			{
				subGroup->transversalSet.erase( iter );
				Permutation identity;
				identity.word = new ElementList;
				subGroup->transversalSet.insert( identity );
				break;
			}
		}

		subGroup = subGroup->subGroup;
	}

	clock_t lastOptimizationTime = clock();
	uint allUnnamedCount = -1;

	Permutation permutation;
	while( permutationStream.OutputPermutation( permutation ) )
	{
		if( group->OptimizeNameWithPermutation( permutation, compressInfo ) )
		{
			lastOptimizationTime = clock();

			allUnnamedCount = group->CountAllUnnamedRepresentatives();
			if( logStream )
				*logStream << "Unnamed count: " << allUnnamedCount << "\n";
		}

		clock_t currentTime = clock();
		double elapsedTimeSec = double( currentTime - lastOptimizationTime ) / double( CLOCKS_PER_SEC );

		// When this count goes to zero, we can bail at any time, but by staying in longer,
		// we can hopefully optimize more representatives.  Ideally we could get all of them
		// to be under a certain word length.
		if( elapsedTimeSec > timeOutSec && allUnnamedCount == 0 )
			break;
	}

	return true;
}

bool StabilizerChain::Group::OptimizeNameWithPermutation( Permutation& permutation, const CompressInfo& compressInfo )
{
	if( !permutation.word )
		return false;

	uint stabilizerPoint = GetSubgroupStabilizerPoint();
	if( permutation.Evaluate( stabilizerPoint ) == stabilizerPoint )
	{
		if( !subGroup )
			return false;

		return subGroup->OptimizeNameWithPermutation( permutation, compressInfo );
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

	product.CompressWord( compressInfo );

	return OptimizeNameWithPermutation( product, compressInfo );
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