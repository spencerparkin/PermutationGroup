// PermutationStream.cpp

#include "PermutationStream.h"

//------------------------------------------------------------------------------------------
//                                 PermutationStream
//------------------------------------------------------------------------------------------

PermutationStream::PermutationStream( void )
{
}

/*virtual*/ PermutationStream::~PermutationStream( void )
{
}

/*virtual*/ bool PermutationStream::Reset( void )
{
	return false;
}

/*virtual*/ bool PermutationStream::OutputPermutation( Permutation& permutation )
{
	return false;
}

/*virtual*/ bool PermutationStream::InputPermutation( const Permutation& permutation )
{
	return false;
}

void PermutationStream::LoadPermutationArray( PermutationArray& permutationArray, int loadMax /*= -1*/ )
{
	int i = 0;
	Permutation permutation;
	while( ( loadMax == -1 || i < loadMax ) && OutputPermutation( permutation ) )
	{
		permutationArray.push_back( permutation );
		i++;
	}
}

void PermutationStream::UnloadPermutationArray( const PermutationArray& permutationArray )
{
	for( uint i = 0; i < permutationArray.size(); i++ )
		InputPermutation( permutationArray[i] );
}

//------------------------------------------------------------------------------------------
//                                PermutationProductStream
//------------------------------------------------------------------------------------------

PermutationProductStream::PermutationProductStream( void )
{
	wrapped = false;
}

/*virtual*/ PermutationProductStream::~PermutationProductStream( void )
{
	Clear();
}

void PermutationProductStream::Clear( void )
{
	for( uint i = 0; i < componentArray.size(); i++ )
		delete componentArray[i].permutationArray;
	componentArray.clear();
}

/*virtual*/ bool PermutationProductStream::Reset( void )
{
	for( uint i = 0; i < componentArray.size(); i++ )
		componentArray[i].offset = 0;
	return true;
}

/*virtual*/ bool PermutationProductStream::OutputPermutation( Permutation& permutation )
{
	permutation.DefineIdentity();
	delete permutation.word;
	permutation.word = new ElementList;
	uint i;
	for( i = 0; i < componentArray.size(); i++ )
		permutation.MultiplyOnRight( *( *componentArray[i].permutationArray )[ componentArray[i].offset ] );

	wrapped = false;
	for( i = 0; i < componentArray.size(); i++ )
	{
		Component* component = &componentArray[i];
		component->offset++;
		if( component->offset < component->permutationArray->size() )
			break;
		component->offset = 0;
	}

	if( i == componentArray.size() )
		wrapped = true;

	return true;
}

void PermutationProductStream::Configure( const StabilizerChain* stabChain )
{
	Clear();

	uint depth = stabChain->Depth();

	for( uint i = 0; i < depth; i++ )
	{
		const StabilizerChain::Group* subGroup = stabChain->GetSubGroupAtDepth( depth - 1 - i );

		Component component;
		component.offset = 0;
		component.permutationArray = new PermutationConstPtrArray;

		for( PermutationSet::const_iterator iter = subGroup->transversalSet.cbegin(); iter != subGroup->transversalSet.cend(); iter++ )
			component.permutationArray->push_back( &( *iter ) );

		componentArray.push_back( component );
	}
}

//------------------------------------------------------------------------------------------
//                              PermutationFreeGroupStream
//------------------------------------------------------------------------------------------

PermutationFreeGroupStream::PermutationFreeGroupStream( const PermutationSet* generatorSet, const CompressInfo* compressInfo )
{
	for( PermutationSet::const_iterator iter = generatorSet->cbegin(); iter != generatorSet->cend(); iter++ )
	{
		const Permutation& generator = *iter;

		Permutation invGenerator;
		generator.GetInverse( invGenerator );

		generatorArray.push_back( generator );
		generatorArray.push_back( invGenerator );
	}

	this->compressInfo = compressInfo;
	productStream = nullptr;
	wordSize = 0;
}

/*virtual*/ PermutationFreeGroupStream::~PermutationFreeGroupStream( void )
{
	delete productStream;
}

/*virtual*/ bool PermutationFreeGroupStream::Reset( void )
{
	delete productStream;
	productStream = nullptr;
	wordSize = 0;
	return true;
}

/*virtual*/ bool PermutationFreeGroupStream::OutputPermutation( Permutation& permutation )
{
	if( !productStream )
	{
		if( wordSize == 0 )
		{
			permutation.DefineIdentity();
			wordSize++;
			return true;
		}
		else
		{
			productStream = new PermutationProductStream();

			for( uint i = 0; i < wordSize; i++ )
			{
				PermutationProductStream::Component component;
				component.offset = 0;
				component.permutationArray = new PermutationConstPtrArray;

				for( uint j = 0; j < generatorArray.size(); j++ )
					component.permutationArray->push_back( &generatorArray[j] );

				productStream->componentArray.push_back( component );
			}

			wordSize++;
		}
	}

	if( !productStream->OutputPermutation( permutation ) )
		return false;

	if( productStream->wrapped )
	{
		delete productStream;
		productStream = nullptr;
	}

	permutation.CompressWord( *compressInfo );
	return true;
}

//------------------------------------------------------------------------------------------
//								  PermutationFifoStream
//------------------------------------------------------------------------------------------

PermutationFifoStream::PermutationFifoStream( void )
{
}

/*virtual*/ PermutationFifoStream::~PermutationFifoStream( void )
{
}

/*virtual*/ bool PermutationFifoStream::OutputPermutation( Permutation& permutation )
{
	if( permutationList.size() == 0 )
		return false;

	permutation = permutationList.back();
	permutationList.pop_back();
	return true;
}

/*virtual*/ bool PermutationFifoStream::InputPermutation( const Permutation& permutation )
{
	permutationList.push_front( permutation );
	return true;
}

//------------------------------------------------------------------------------------------
//                               PermutationWordStream
//------------------------------------------------------------------------------------------

PermutationWordStream::PermutationWordStream( const PermutationSet* generatorSet, const CompressInfo* compressInfo )
{
	this->generatorSet = generatorSet;
	this->compressInfo = compressInfo;

	Reset();
}

/*virtual*/ PermutationWordStream::~PermutationWordStream( void )
{
}

/*virtual*/ bool PermutationWordStream::Reset( void )
{
	processedSet.clear();
	permutationQueue.clear();

	Permutation identity;
	identity.word = new ElementList;
	permutationQueue.insert( identity );

	return true;
}

/*virtual*/ bool PermutationWordStream::OutputPermutation( Permutation& permutation )
{
	if( permutationQueue.size() == 0 )
		return false;

	OrderedPermutationSet::iterator iter = permutationQueue.begin();
	permutation = *iter;
	permutationQueue.erase( iter );

	processedSet.insert( permutation );

	for( PermutationSet::const_iterator genIter = generatorSet->cbegin(); genIter != generatorSet->cend(); genIter++ )
	{
		const Permutation& generator = *genIter;

		Permutation newPermutation;
		newPermutation.Multiply( permutation, generator );

		newPermutation.CompressWord( *compressInfo );

		if( processedSet.find( newPermutation ) == processedSet.end() &&
			permutationQueue.find( newPermutation ) == permutationQueue.end() )
		{
			permutationQueue.insert( newPermutation );
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------
//                                PermutationRandomStream
//------------------------------------------------------------------------------------------

PermutationRandomStream::PermutationRandomStream( const PermutationSet* generatorSet, const CompressInfo* compressInfo ) : conjugateStream( generatorSet, compressInfo ), nonCommutatorStream( generatorSet, compressInfo )
{
	maxCommutatorDepth = 4;
	maxConjugateCount = 32;
	currentConjugateCount = 1024;
}

/*virtual*/ PermutationRandomStream::~PermutationRandomStream( void )
{
}

/*virtual*/ bool PermutationRandomStream::OutputPermutation( Permutation& permutation )
{
	if( currentConjugateCount >= maxConjugateCount )
	{
		for( uint i = 0; i < 32; i++ )
		{
			if( !nonCommutatorStream.OutputPermutation( permutation ) )
				return false;
			nonCommutatorPool.push_back( permutation );
		}

		GenerateRandomCommutator( randomCommutator, RandomInteger( 0, maxCommutatorDepth ) );
		currentConjugateCount = 0;
		conjugateStream.Reset();
	}

	Permutation conjugator;
	if( !conjugateStream.OutputPermutation( conjugator ) )
		return false;

	Permutation invConjugator;
	conjugator.GetInverse( invConjugator );

	permutation.Multiply( conjugator, randomCommutator );
	permutation.MultiplyOnRight( invConjugator );

	currentConjugateCount++;
	return true;
}

uint PermutationRandomStream::RandomInteger( uint min, uint max )
{
	uint r = rand();
	double t = double(r) / double( RAND_MAX );
	r = uint( double( min ) + t * double( max - min ) );
	if( r < min )
		r = min;
	if( r > max )
		r = max;
	return r;
}

void PermutationRandomStream::GenerateRandomCommutator( Permutation& commutator, uint depth )
{
	if( depth == 0 )
	{
		uint i = RandomInteger( 0, nonCommutatorPool.size() - 1 );
		commutator.SetCopy( nonCommutatorPool[i] );
	}
	else
	{
		do
		{
			uint flags = RandomInteger( 1, 3 );
		
			Permutation permutationA, permutationB;

			if( flags & 1 )
				GenerateRandomCommutator( permutationA, depth - 1 );
			else
				GenerateRandomCommutator( permutationA, 0 );

			if( flags & 2 )
				GenerateRandomCommutator( permutationB, depth - 1 );
			else
				GenerateRandomCommutator( permutationB, 0 );

			Permutation invPermutationA, invPermutationB;

			permutationA.GetInverse( invPermutationA );
			permutationB.GetInverse( invPermutationB );

			Permutation productA, productB;

			productA.Multiply( permutationA, permutationB );
			productB.Multiply( invPermutationA, invPermutationB );

			commutator.Multiply( productA, productB );
		}
		while( commutator.IsIdentity() );
	}
}

// PermutationStream.cpp