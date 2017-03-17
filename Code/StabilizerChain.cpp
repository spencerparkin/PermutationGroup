// StabilizerChain.cpp

#include "StabilizerChain.h"
#include <memory>
#include <rapidjson/writer.h>

//------------------------------------------------------------------------------------------
//                                   StabilizerChainGroup
//------------------------------------------------------------------------------------------

StabilizerChainGroup::StabilizerChainGroup( void )
{
	subGroup = nullptr;

	stabilizerPoint = 0;
}

StabilizerChainGroup::~StabilizerChainGroup( void )
{
	delete subGroup;
}

bool StabilizerChainGroup::LoadFromJsonString( const std::string& jsonString )
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

bool StabilizerChainGroup::SaveToJsonString( std::string& jsonString, uint flags /*= FLAG_REPRESENTATIVES*/ ) const
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

bool StabilizerChainGroup::LoadRecursive( /*const*/ rapidjson::Value& chainGroupValue )
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
		subGroup = new StabilizerChainGroup();

		rapidjson::Value subGroupValue = chainGroupValue[ "subGroup" ].GetObject();
		if( !subGroup->LoadRecursive( subGroupValue ) )
			return false;
	}

	return true;
}

bool StabilizerChainGroup::SaveRecursive( rapidjson::Value& chainGroupValue, rapidjson::Document::AllocatorType& allocator, uint flags ) const
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

/*static*/ bool StabilizerChainGroup::LoadPermutationSet( PermutationSet& permutationSet, /*const*/ rapidjson::Value& arrayValue )
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

/*static*/ bool StabilizerChainGroup::SavePermutationSet( const PermutationSet& permutationSet, rapidjson::Value& arrayValue, rapidjson::Document::AllocatorType& allocator )
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

void StabilizerChainGroup::Print( std::ostream& ostream, uint flags /*= FLAG_REPRESENTATIVES*/ ) const
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
bool StabilizerChainGroup::Generate( uint* pointArray, uint pointArraySize, uint pointArrayOffset, bool generateWords, std::ostream* ostream /*= nullptr*/ )
{
	if( generatorSet.size() == 1 )
	{
		PermutationSet::iterator iter = generatorSet.begin();
		const Permutation& permutation = *iter;
		if( permutation.IsIdentity() )
		{
			if( ostream )
				*ostream << "Process complete!\n";

			return true;
		}
	}

	if( pointArrayOffset >= pointArraySize )
		return false;

	stabilizerPoint = pointArray[ pointArrayOffset ];

	if( ostream )
	{
		*ostream << "Calculating orbits of " << stabilizerPoint << " in group generated by...\n";
		
		for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
			( *iter ).Print( *ostream );

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

	subGroup = new StabilizerChainGroup();

	if( !CalculateSchreierGenerators( subGroup->generatorSet ) )
		return false;

	if( !subGroup->Generate( pointArray, pointArraySize, pointArrayOffset + 1, generateWords, ostream ) )
		return false;
	
	return true;
}

bool StabilizerChainGroup::CalculateSchreierGenerators( PermutationSet& schreierGeneratorSet )
{
	for( PermutationSet::iterator genIter = generatorSet.begin(); genIter != generatorSet.end(); genIter++ )
	{
		const Permutation& generator = *genIter;

		for( PermutationSet::iterator transIter = transversalSet.begin(); transIter != transversalSet.end(); transIter++ )
		{
			const Permutation& cosetRepresentative = *transIter;

			Permutation product;
			product.Multiply( generator, cosetRepresentative );

			PermutationSet::iterator iter = FindCoset( product );
			if( iter == transversalSet.end() )
				return false;		// Something went wrong with our math!

			Permutation invCosetRepresentative;
			invCosetRepresentative.SetInverse( *iter );

			Permutation newGenerator;
			newGenerator.Multiply( product, invCosetRepresentative );

			schreierGeneratorSet.insert( newGenerator );
		}
	}

	return true;
}

PermutationSet::iterator StabilizerChainGroup::FindCoset( const Permutation& permutation )
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

bool StabilizerChainGroup::FactorInverse( const Permutation& permutation, Permutation& invPermutation ) const
{
	if( !subGroup )
		return false;

	if( permutation.Evaluate( stabilizerPoint ) == stabilizerPoint )
		return subGroup->FactorInverse( permutation, invPermutation );

	PermutationSet::iterator iter = const_cast< StabilizerChainGroup* >( this )->FindCoset( permutation );
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

uint StabilizerChainGroup::CountUnnamedRepresentatives( void ) const
{
	uint count = 0;

	for( PermutationSet::const_iterator iter = transversalSet.cbegin(); iter != transversalSet.cend(); iter++ )
	{
		const Permutation& permutation = *iter;
		if( !permutation.word )
			count++;
	}

	if( subGroup )
		count += subGroup->CountUnnamedRepresentatives();

	return count;
}

// This may be the naive approach to what Minkwitz describes in his paper.
bool StabilizerChainGroup::Optimize( std::ostream* ostream /*= nullptr*/ )
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

	PermutationSet processedSet;
	PermutationSet permutationQueue;
	Permutation identity;
	identity.word = new ElementList;
	permutationQueue.insert( identity );

	// TODO: This will iterate over all members of the group.  Now, if we actually had the
	//       time and memory to do that, then we wouldn't need the stabilizer chain.  So
	//       what we need here is an earily-out.  If the maximum word-size of any coset
	//       representative goes below a given threshold, then this might be a good reason
	//       to bail.
	while( permutationQueue.size() > 0 )
	{
		iter = permutationQueue.begin();
		Permutation permutation = *iter;
		permutationQueue.erase( iter );

		if( ostream )
		{
			*ostream << "Trying...\n";
			permutation.Print( *ostream );
		}

		if( OptimizeWithPermutation( permutation, ostream ) )
		{
			uint unnamedCount = CountUnnamedRepresentatives();
			if( ostream )
				*ostream << "Unnamed count: " << unnamedCount << "\n";
			if( unnamedCount == 0 )
				break;
		}

		processedSet.insert( permutation );
		EnqueueNewPermutations( permutation, permutationQueue, &processedSet );
	}

	return true;
}

bool StabilizerChainGroup::OptimizeWithPermutation( const Permutation& permutation, std::ostream* ostream /*= nullptr*/ )
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

void StabilizerChainGroup::EnqueueNewPermutations( const Permutation& permutation, PermutationSet& permutationQueue, PermutationSet* processedSet /*= nullptr*/ )
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

// StabilizerChain.cpp