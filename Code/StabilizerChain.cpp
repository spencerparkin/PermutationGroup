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

StabilizerChain* StabilizerChain::Clone( void ) const
{
	std::string jsonString;
	if( !SaveToJsonString( jsonString ) )
		return nullptr;

	StabilizerChain* stabChain = new StabilizerChain();
	if( !stabChain->LoadFromJsonString( jsonString ) )
	{
		delete stabChain;
		return nullptr;
	}

	return stabChain;
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

const StabilizerChain::Group* StabilizerChain::GetSubGroupAtDepth( uint depth ) const
{
	return const_cast< StabilizerChain* >( this )->GetSubGroupAtDepth( depth );
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
	{
		NaturalNumberSet stabilizerPointSet;
		stabilizerPointSet.SetFromJsonValue( baseArrayValue[i] );
		baseArray.push_back( stabilizerPointSet );
	}

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
	{
		rapidjson::Value setValue( rapidjson::kArrayType );
		baseArray[i].GetToJsonValue( setValue, doc.GetAllocator() );
		baseArrayValue.PushBack( setValue, doc.GetAllocator() );
	}

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
	{
		NaturalNumberSet singletonSet;
		singletonSet.AddMember( baseArray[i] );
		this->baseArray.push_back( singletonSet );
	}

	delete group;
	group = new Group( this, nullptr, 0 );

	if( logStream )
		*logStream << "Generating stabilizer chain!!!\n";

	uint size = ( uint )generatorSet.size();
	uint count = 0;

	for( PermutationSet::const_iterator iter = generatorSet.cbegin(); iter != generatorSet.cend(); iter++ )
	{
		const Permutation& generator = *iter;

		if( !generator.IsValid() )
			return false;

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

	// I think that these occur due to an overly sufficient base.
	while( true )
	{
		subGroup = group;
		while( subGroup && subGroup->transversalSet.size() != 1 )
			subGroup = subGroup->subGroup;

		if( !subGroup || subGroup == group )
			break;

		if( !subGroup->Eliminate() )
			return false;

		delete subGroup;
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
	ostream << "Group has sub-group stabilizing:\n";

	GetSubgroupStabilizerPointSet().Print( ostream );

	ostream << "Generator set:\n";

	for( PermutationSet::const_iterator iter = generatorSet.cbegin(); iter != generatorSet.cend(); iter++ )
		( *iter ).Print( ostream );

	ostream << "Transversal set:\n";

	for( PermutationSet::const_iterator iter = transversalSet.cbegin(); iter != transversalSet.cend(); iter++ )
		( *iter ).Print( ostream );

	if( subGroup )
		subGroup->Print( ostream );
}

const NaturalNumberSet& StabilizerChain::Group::GetSubgroupStabilizerPointSet( void ) const
{
	return stabChain->baseArray[ stabilizerOffset ];
}

bool StabilizerChain::Group::Eliminate( void )
{
	if( !subGroup )
		return false;

	const NaturalNumberSet& sourcePointSet = GetSubgroupStabilizerPointSet();
	NaturalNumberSet& destinationPointSet = stabChain->baseArray[ subGroup->stabilizerOffset ];
	for( NaturalNumberSet::UintSet::const_iterator iter = sourcePointSet.set.cbegin(); iter != sourcePointSet.set.cend(); iter++ )
		destinationPointSet.AddMember( *iter );

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
	return true;
}

// Note: Here we're limited to integer arithematic mod 2^64, which is not sufficient
//       to calculate the order of the Rubik's Cube group!
unsigned long long StabilizerChain::Group::Order( void ) const
{
	unsigned long long subGroupIndex = transversalSet.size();
	unsigned long long subGroupOrder = 1;
	if( subGroup )
		subGroupOrder = subGroup->Order();
	unsigned long long groupOrder = subGroupIndex * subGroupOrder;
	return groupOrder;
}

bool StabilizerChain::Group::Extend( const Permutation& generator, bool* extended /*= nullptr*/ )
{
	if( extended )
		*extended = false;

	if( IsMember( generator ) )
		return true;

	if( extended )
		*extended = true;

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
		const NaturalNumberSet& stabilizerPointSet = GetSubgroupStabilizerPointSet();
		if( !stabilizerPointSet.Cardinality() == 1 )
			return false;

		// The root of the orbit tree must be the identity to satisfy a requirement of Schreier's lemma.
		Permutation identity;
		transversalSet.insert( identity );
		PermutationSet::iterator iter = transversalSet.find( identity );
		rootNode = new OrbitNode( &( *iter ) );
		orbitSet.AddMember( *stabilizerPointSet.set.begin() );
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
	const NaturalNumberSet& stabilizerPointSet = GetSubgroupStabilizerPointSet();

	Permutation invPermutation;
	invPermutation.SetInverse( permutation );

	PermutationSet::iterator iter = transversalSet.begin();
	while( iter != transversalSet.end() )
	{
		const Permutation& cosetRepresentative = *iter;

		Permutation product;
		product.Multiply( cosetRepresentative, invPermutation );

		if( product.Stabilizes( stabilizerPointSet ) )
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
	const NaturalNumberSet& stabilizerPointSet = GetSubgroupStabilizerPointSet();
	if( permutation.Stabilizes( stabilizerPointSet ) )
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
// be found in a paper by Egner and Puschel.  For example, if you can find just one word in
// a transversal set, then you can find words for all the others using the orbit-stabilizer
// theorem, provided you know the generator factorizations, although these would grow in
// length the further they are from the root.
bool StabilizerChain::OptimizeNames( PermutationStream& permutationStream, const CompressInfo& compressInfo, double timeOutSec /*= 60.0*/, OptimizeNamesCallback callback /*= nullptr*/, void* callback_data /*= nullptr*/ )
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
	Stats stats;

	Permutation permutation;
	while( permutationStream.OutputPermutation( permutation ) )
	{
		if( group->OptimizeNameWithPermutation( permutation, compressInfo ) )
		{
			lastOptimizationTime = clock();

			stats.Reset();
			group->AccumulateStats( stats );

			if( logStream )
				stats.Print( *logStream );
		}

		if( callback && callback( stats, callback_data ) )
			break;

		clock_t currentTime = clock();
		double elapsedTimeSec = double( currentTime - lastOptimizationTime ) / double( CLOCKS_PER_SEC );

		if( elapsedTimeSec > timeOutSec && stats.totalUnnamedTransversalCount == 0 )
			break;
	}

	return ( stats.totalUnnamedTransversalCount == 0 ) ? true : false;
}

bool StabilizerChain::Group::OptimizeNameWithPermutation( Permutation& permutation, const CompressInfo& compressInfo )
{
	if( !permutation.word )
		return false;

	const NaturalNumberSet& stabilizerPointSet = GetSubgroupStabilizerPointSet();
	if( permutation.Stabilizes( stabilizerPointSet ) )
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

StabilizerChain::Stats::Stats( void )
{
	Reset();
}

void StabilizerChain::Stats::Reset( void )
{
	totalUnnamedGeneratorCount = 0;
	totalUnnamedTransversalCount = 0;

	unnamedGeneratorCountArray.clear();
	unnamedTransversalCountArray.clear();
}

void StabilizerChain::Stats::Print( std::ostream& ostream ) const
{
	ostream << "Total unnamed transversal elements: " << totalUnnamedTransversalCount << "\n";
	//ostream << "Total unnamed generator elements: " << totalUnnamedGeneratorCount << "\n";

	for( uint i = 0; i < unnamedTransversalCountArray.size(); i++ )
		ostream << unnamedTransversalCountArray[i] << " unnamed transversal elements at level " << i << "\n";

	//for( uint i = 0; i < unnamedGeneratorCountArray.size(); i++ )
	//	ostream << unnamedGeneratorCountArray[i] << " unnamed generator elements at level " << i << "\n";
}

void StabilizerChain::Group::AccumulateStats( Stats& stats ) const
{
	uint unnamedTransversalCount = 0;
	for( PermutationSet::const_iterator iter = transversalSet.cbegin(); iter != transversalSet.cend(); iter++ )
	{
		const Permutation& permutation = *iter;
		if( !permutation.word )
			unnamedTransversalCount++;
	}
	
	uint unnamedGeneratorCount = 0;
	for( PermutationSet::const_iterator iter = generatorSet.cbegin(); iter != generatorSet.cend(); iter++ )
	{
		const Permutation& permutation = *iter;
		if( !permutation.word )
			unnamedGeneratorCount++;
	}

	stats.unnamedTransversalCountArray.push_back( unnamedTransversalCount );
	stats.unnamedGeneratorCountArray.push_back( unnamedGeneratorCount );
	stats.totalUnnamedTransversalCount += unnamedTransversalCount;
	stats.totalUnnamedGeneratorCount += unnamedGeneratorCount;

	if( subGroup )
		subGroup->AccumulateStats( stats );
}

bool StabilizerChain::Group::IsSubGroupOf( const Group& group ) const
{
	for( PermutationSet::const_iterator iter = generatorSet.cbegin(); iter != generatorSet.cend(); iter++ )
	{
		const Permutation& generator = *iter;
		if( !group.IsMember( generator ) )
			return false;
	}

	return true;
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
	// The orbit-stabilizer theorem does not generalize to stabilizer subgroups of multiple points.
	// I did, however, find a generalization of the orbit-stabilizer theorem for permutations
	// that stabilize a set of points.  Unfortunately, I couldn't see how that could be useful to me.
	// In any case, the chain can be constructed in the traditional manner, then shortened to get the
	// desired result anyway.
	const NaturalNumberSet& stabilizerPointSet = group->GetSubgroupStabilizerPointSet();
	if( stabilizerPointSet.Cardinality() != 1 )
		return false;

	uint stabilizerPoint = *stabilizerPointSet.set.begin();

	const PermutationSet* permutationSet = &generatorSet;
	if( !fresh )
		permutationSet = &singletonSet;

	uint nonFreshSize = ( uint )adjacentNodeArray.size();

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