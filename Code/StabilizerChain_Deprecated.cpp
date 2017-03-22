// StabilizerChain_Deprecated.cpp

#include "StabilizerChain_Deprecated.h"
#include <memory>
#include <time.h>
#include <rapidjson/writer.h>

//------------------------------------------------------------------------------------------
//                                   StabilizerChainGroup_Deprecated
//------------------------------------------------------------------------------------------

StabilizerChainGroup_Deprecated::StabilizerChainGroup_Deprecated( void )
{
	subGroup = nullptr;

	stabilizerPoint = 0;
}

StabilizerChainGroup_Deprecated::~StabilizerChainGroup_Deprecated( void )
{
	delete subGroup;
}

bool StabilizerChainGroup_Deprecated::LoadFromJsonString( const std::string& jsonString )
{
	generatorSet.clear();
	transversalSet.clear();

	delete subGroup;
	subGroup = nullptr;

	rapidjson::Document doc;
	doc.Parse( jsonString.c_str() );
	if( !doc.IsObject() )
		return false;

	if( !doc.HasMember( "group" ) )
		return false;

	rapidjson::Value chainGroupValue = doc[ "group" ].GetObject();
	if( !LoadRecursive( chainGroupValue ) )
		return false;

	return true;
}

bool StabilizerChainGroup_Deprecated::SaveToJsonString( std::string& jsonString, uint flags /*= FLAG_REPRESENTATIVES | FLAG_GENERATORS*/ ) const
{
	rapidjson::Document doc;
	doc.SetObject();

	rapidjson::Value chainGroupValue( rapidjson::kObjectType );
	if( !SaveRecursive( chainGroupValue, doc.GetAllocator(), flags ) )
		return false;

	doc.AddMember( "group", chainGroupValue, doc.GetAllocator() );

	rapidjson::StringBuffer buffer;
	rapidjson::Writer< rapidjson::StringBuffer > writer( buffer );
	if( !doc.Accept( writer ) )
		return false;

	jsonString = buffer.GetString();
	return true;
}

bool StabilizerChainGroup_Deprecated::LoadRecursive( /*const*/ rapidjson::Value& chainGroupValue )
{
	if( !chainGroupValue.HasMember( "stabilizerPoint" ) )
		return false;

	stabilizerPoint = chainGroupValue[ "stabilizerPoint" ].GetInt();

	if( chainGroupValue.HasMember( "generators" ) )
	{
		rapidjson::Value generatorsValue = chainGroupValue[ "generators" ].GetArray();
		if( !LoadPermutationSet( generatorSet, generatorsValue ) )
			return false;
	}

	if( chainGroupValue.HasMember( "transversals" ) )
	{
		rapidjson::Value transversalsValue = chainGroupValue[ "transversals" ].GetArray();
		if( !LoadPermutationSet( transversalSet, transversalsValue ) )
			return false;
	}

	if( chainGroupValue.HasMember( "subGroup" ) )
	{
		subGroup = new StabilizerChainGroup_Deprecated();

		rapidjson::Value subGroupValue = chainGroupValue[ "subGroup" ].GetObject();
		if( !subGroup->LoadRecursive( subGroupValue ) )
			return false;
	}

	return true;
}

bool StabilizerChainGroup_Deprecated::SaveRecursive( rapidjson::Value& chainGroupValue, rapidjson::Document::AllocatorType& allocator, uint flags ) const
{
	chainGroupValue.AddMember( "stabilizerPoint", stabilizerPoint, allocator );

	if( flags & FLAG_GENERATORS )
	{
		rapidjson::Value generatorsValue( rapidjson::kArrayType );
		if( !SavePermutationSet( generatorSet, generatorsValue, allocator ) )
			return false;

		chainGroupValue.AddMember( "generators", generatorsValue, allocator );
	}

	if( flags & FLAG_REPRESENTATIVES )
	{
		rapidjson::Value transversalsValue( rapidjson::kArrayType );
		if( !SavePermutationSet( transversalSet, transversalsValue, allocator ) )
			return false;

		chainGroupValue.AddMember( "transversals", transversalsValue, allocator );
	}

	if( subGroup )
	{
		rapidjson::Value subGroupValue( rapidjson::kObjectType );
		if( !subGroup->SaveRecursive( subGroupValue, allocator, flags ) )
			return false;

		chainGroupValue.AddMember( "subGroup", subGroupValue, allocator );
	}

	return true;
}

/*static*/ bool StabilizerChainGroup_Deprecated::LoadPermutationSet( PermutationSet& permutationSet, /*const*/ rapidjson::Value& arrayValue )
{
	permutationSet.clear();

	for( uint i = 0; i < arrayValue.Size(); i++ )
	{
		rapidjson::Value permutationValue = arrayValue[i].GetObject();

		Permutation permutation;
		if( !permutation.SetFromJsonValue( permutationValue ) )
			return false;

		permutationSet.insert( permutation );
	}

	return true;
}

/*static*/ bool StabilizerChainGroup_Deprecated::SavePermutationSet( const PermutationSet& permutationSet, rapidjson::Value& arrayValue, rapidjson::Document::AllocatorType& allocator )
{
	for( PermutationSet::const_iterator iter = permutationSet.cbegin(); iter != permutationSet.cend(); iter++ )
	{
		rapidjson::Value permutationValue( rapidjson::kObjectType );
		if( !( *iter ).GetToJsonValue( permutationValue, allocator ) )
			return false;

		arrayValue.PushBack( permutationValue, allocator );
	}

	return true;
}

void StabilizerChainGroup_Deprecated::Print( std::ostream& ostream, uint flags /*= FLAG_REPRESENTATIVES*/ ) const
{
	ostream << "==================================================================\n";
	ostream << "Stabilizer point for subgroup is: " << stabilizerPoint << "\n";

	if( flags & FLAG_GENERATORS )
	{
		ostream << "--------------------------------------------\n";
		ostream << "Generators...\n";
		for( PermutationSet::const_iterator iter = generatorSet.cbegin(); iter != generatorSet.cend(); iter++ )
			( *iter ).Print( ostream );
	}

	if( flags & FLAG_REPRESENTATIVES )
	{
		ostream << "--------------------------------------------\n";
		ostream << "Coset representatives...\n";
		for( PermutationSet::const_iterator iter = transversalSet.cbegin(); iter != transversalSet.cend(); iter++ )
			( *iter ).Print( ostream );
	}

	if( subGroup )
		subGroup->Print( ostream, flags );
}

// The order in which we stabilize points may have an impact on the size of the factorizations in the chain.
bool StabilizerChainGroup_Deprecated::Generate( uint* pointArray, uint pointArraySize, uint pointArrayOffset, bool generateWords, std::ostream* ostream /*= nullptr*/ )
{
	if( generatorSet.size() == 0 )
	{
		if( ostream )
			*ostream << "Process complete!\n";

		return true;
	}

	if( pointArrayOffset >= pointArraySize )
		return false;

	stabilizerPoint = pointArray[ pointArrayOffset ];

	if( ostream )
	{
		*ostream << "Calculating orbits of " << stabilizerPoint << " in group generated by...\n";
		
		//for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
		//	( *iter ).Print( *ostream );

		*ostream << "Total generators: " << generatorSet.size() << "\n";
	}

	NaturalNumberSet orbitSet;
	PermutationSet permutationQueue;
	Permutation identity;
	
	if( generateWords )
		identity.word = new ElementList;

	permutationQueue.insert( identity );

	// Notice that the identity element will always land in the traversal.
	// This is a requirement of Schreier's lemma.
	while( permutationQueue.size() > 0 )
	{
		PermutationSet::iterator iter = permutationQueue.begin();
		Permutation permutation = *iter;
		permutationQueue.erase( iter );

		uint point = permutation.Evaluate( stabilizerPoint );
		if( !orbitSet.IsMember( point ) )
		{
			if( ostream )
			{
				*ostream << "Found new orbit: " << point << "\n";
				*ostream << "Coset representative: ";
				permutation.Print( *ostream );
			}

			orbitSet.AddMember( point );
			
			// This is the orbit-stabilizer theorem!
			transversalSet.insert( permutation );

			EnqueueNewPermutations( permutation, permutationQueue );
		}
	}

	if( ostream )
		*ostream << "Calculating Schreier generators...\n";

	subGroup = new StabilizerChainGroup_Deprecated();

	if( !CalculateSchreierGenerators( subGroup->generatorSet ) )
		return false;

	if( !subGroup->Generate( pointArray, pointArraySize, pointArrayOffset + 1, generateWords, ostream ) )
		return false;
	
	return true;
}

// After doing some research online, I'm still doing things the naive way!
// The Schreier-Sims ways of generating the chain is to actually build it from
// the bottom up.  The beauty of this is that we can use the partially constructed
// change to weed out unnecessary Schreier generators.  That's brilliant!  But I
// need to wrap my head around how we would build the chain bottom-up instead of
// top-down.  It is not immediately obvious to me.  Given a full chain, not necessarily
// for the group we ultimate care about, we can use it to test for membership in that group.
// To weed out a reduntant Schreier generator, we have to have a full chain for the group
// generated by how many such generators we have so far.  If the Schreier generator is already
// in the group, we can ignore it.  If it's not, we have to extend the chain somehow.
// However we do that, we must end up with another full chain.  This is all quite fuzzy,
// and not obvious to me at all.
//
// Alternative to Schreier-Sims might be the problem of generating a BSGS - base with strong
// generating set.  It's not too hard to see how we can construct the chain form the SGS,
// and if that SGS isn't too large (which is the problem we're trying to solve: reducing
// unnecessary generators), then the construction will go smoothly.  This idea lets us continue
// to build the chain top-down, instead of bottom-up, which I can't seem to image being able
// to do for the time being.
//
// From what I've found, finding a BSGS is done using Schreier-Sims.  :/  I need to figure that out.
bool StabilizerChainGroup_Deprecated::CalculateSchreierGenerators( PermutationSet& schreierGeneratorSet )
{
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

			Permutation newGenerator;
			newGenerator.Multiply( product, invCosetRepresentative );

			if( !newGenerator.IsIdentity() )
				schreierGeneratorSet.insert( newGenerator );
		}
	}

	return true;
}

PermutationSet::iterator StabilizerChainGroup_Deprecated::FindCoset( const Permutation& permutation )
{
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

bool StabilizerChainGroup_Deprecated::FactorInverse( const Permutation& permutation, Permutation& invPermutation ) const
{
	if( !subGroup )
		return true;

	if( permutation.Evaluate( stabilizerPoint ) == stabilizerPoint )
		return subGroup->FactorInverse( permutation, invPermutation );

	PermutationSet::iterator iter = const_cast< StabilizerChainGroup_Deprecated* >( this )->FindCoset( permutation );
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

uint StabilizerChainGroup_Deprecated::CountUnnamedRepresentatives( void ) const
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

uint StabilizerChainGroup_Deprecated::CountAllUnnamedRepresentatives( void ) const
{
	uint count = CountUnnamedRepresentatives();

	if( subGroup )
		count += subGroup->CountAllUnnamedRepresentatives();

	return count;
}

bool StabilizerChainGroup_Deprecated::MakeCompressInfo( CompressInfo& compressInfo )
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

void StabilizerChainGroup_Deprecated::NameGenerators( void )
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

// The main idea here is taken from Minkwitz, although I'm sure this isn't exactly what he had in mind.
// The idea of replacing transversal elements is most certainly taken from him.
bool StabilizerChainGroup_Deprecated::Optimize( const CompressInfo& compressInfo, std::ostream* ostream /*= nullptr*/ )
{
	StabilizerChainGroup_Deprecated* group = subGroup;
	while( group )
	{
		group->generatorSet.clear();
		group = group->subGroup;
	}

	group = this;
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

	uint allUnnamedCount = -1;

	while( permutationQueue.size() > 0 )
	{
		PermutationSet::iterator iter = permutationQueue.begin();
		Permutation permutation = *iter;
		permutationQueue.erase( iter );

		if( ostream )
		{
			//*ostream << "Trying...\n";
			//permutation.Print( *ostream );
		}

		permutation.CompressWord( compressInfo );

		if( OptimizeWithPermutation( permutation, ostream ) )
		{
			lastOptimizationTime = clock();

			allUnnamedCount = CountAllUnnamedRepresentatives();
			if( ostream )
				*ostream << "Unnamed count: " << allUnnamedCount << "\n";
		}

		processedSet.insert( permutation );
		EnqueueNewPermutations( permutation, permutationQueue, &processedSet );

		clock_t currentTime = clock();
		double elapsedTimeSec = double( currentTime - lastOptimizationTime ) / double( CLOCKS_PER_SEC );

		if( elapsedTimeSec > 20.0 && CountUnnamedRepresentatives() == 0 )
			break;

		// When this count goes to zero, we can bail at any time, but by staying in longer,
		// we can hopefully optimize more representatives.  Ideally we could get all of them
		// to be under a certain word length.
		if( elapsedTimeSec > 20.0 && allUnnamedCount == 0 )
			break;
	}

	processedSet.clear();
	permutationQueue.clear();

	uint unnamedCount = CountAllUnnamedRepresentatives();
	if( unnamedCount > 0 )
	{
		// TODO: This code hasn't yet been tested.
		StabilizerChainGroup_Deprecated* group = this;
		while( group && group->CountUnnamedRepresentatives() == 0 )
		{
			if( !group->subGroup )
				return false;

			group->CalculateSchreierGenerators( group->subGroup->generatorSet );
			group = group->subGroup;
		}

		if( !group )
			return false;
		
		return group->Optimize( compressInfo, ostream );
	}

	return true;
}

bool StabilizerChainGroup_Deprecated::OptimizeWithPermutation( const Permutation& permutation, std::ostream* ostream /*= nullptr*/ )
{
	if( !permutation.word )
		return false;

	if( !subGroup )
		return false;

	if( permutation.Evaluate( stabilizerPoint ) == stabilizerPoint )
		return subGroup->OptimizeWithPermutation( permutation, ostream );

	PermutationSet::iterator iter = FindCoset( permutation );
	if( iter == transversalSet.end() )
		return false;

	const Permutation& cosetRepresentative = *iter;

	if( ( cosetRepresentative.word && permutation.word->size() < cosetRepresentative.word->size() ) || !cosetRepresentative.word )
	{
		if( ostream )
		{
			*ostream << "Replacing...\n";
			cosetRepresentative.Print( *ostream );
			*ostream << "...with...\n";
			permutation.Print( *ostream );
		}

		transversalSet.erase( iter );
		transversalSet.insert( permutation );
		return true;
	}

	Permutation invPermutation;
	permutation.GetInverse( invPermutation );

	Permutation product;
	product.Multiply( cosetRepresentative, invPermutation );

	return subGroup->OptimizeWithPermutation( product, ostream );
}

void StabilizerChainGroup_Deprecated::EnqueueNewPermutations( const Permutation& permutation, PermutationSet& permutationQueue, PermutationSet* processedSet /*= nullptr*/ )
{
	for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
	{
		const Permutation& generator = *iter;

		Permutation newPermutation;
		newPermutation.Multiply( permutation, generator );

		if( !processedSet || processedSet->find( newPermutation ) == processedSet->end() )
		{
			if( permutationQueue.find( newPermutation ) == permutationQueue.end() )
				permutationQueue.insert( newPermutation );
		}
	}
}

// StabilizerChain_Deprecated.cpp