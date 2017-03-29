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
//                               PermutationStabGroupStream
//------------------------------------------------------------------------------------------

PermutationStabGroupStream::PermutationStabGroupStream( StabilizerChain::Group* group )
{
	this->group = group;
}

/*virtual*/ PermutationStabGroupStream::~PermutationStabGroupStream( void )
{
}

//------------------------------------------------------------------------------------------
//                               PermutationWordStream
//------------------------------------------------------------------------------------------

PermutationWordStream::PermutationWordStream( StabilizerChain::Group* group, const CompressInfo* compressInfo ) : PermutationStabGroupStream( group )
{
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

	for( PermutationSet::const_iterator genIter = group->generatorSet.cbegin(); genIter != group->generatorSet.cend(); genIter++ )
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
//                                PermutationConjugateStream
//------------------------------------------------------------------------------------------

PermutationConjugateStream::PermutationConjugateStream( StabilizerChain::Group* group, const CompressInfo* compressInfo ) : PermutationWordStream( group, compressInfo )
{
	i = permutationArray.size();
}

/*virtual*/ PermutationConjugateStream::~PermutationConjugateStream( void )
{
}

/*virtual*/ bool PermutationConjugateStream::Reset( void )
{
	if( !PermutationWordStream::Reset() )
		return false;

	i = permutationArray.size();
	return true;
}

/*virtual*/ bool PermutationConjugateStream::OutputPermutation( Permutation& permutation )
{
	do
	{
		if( i >= permutationArray.size() )
		{
			do
			{
				if( !PermutationWordStream::OutputPermutation( conjugatingPermutation ) )
					return false;
			}
			while( conjugatingPermutation.IsIdentity() );

			conjugatingPermutation.GetInverse( invConjugatingPermutation );

			i = 0;
		}

		const Permutation& conjugatedPermutation = permutationArray[i];

		Permutation product;
		product.Multiply( conjugatingPermutation, conjugatedPermutation );

		permutation.Multiply( product, invConjugatingPermutation );

		i++;
	}
	while( permutation.IsIdentity() );

	return true;
}

//------------------------------------------------------------------------------------------
//                                PermutationCommutatorStream
//------------------------------------------------------------------------------------------

PermutationCommutatorStream::PermutationCommutatorStream( void )
{
	Reset();
}

/*virtual*/ PermutationCommutatorStream::~PermutationCommutatorStream( void )
{
}

/*virtual*/ bool PermutationCommutatorStream::Reset( void )
{
	i = j = 0;
	return true;
}

/*virtual*/ bool PermutationCommutatorStream::OutputPermutation( Permutation& permutation )
{
	do
	{
		if( i >= permutationArray.size() )
		{
			if( j >= permutationArray.size() )
				return false;

			i = 0;
			j++;
		}

		const Permutation& permutationA = permutationArray[i];
		const Permutation& permutationB = permutationArray[j];

		Permutation invPermutationA, invPermutationB;

		permutationA.GetInverse( invPermutationA );
		permutationB.GetInverse( invPermutationB );

		Permutation productA, productB;

		productA.Multiply( permutationA, permutationB );
		productB.Multiply( invPermutationA, invPermutationB );

		permutation.Multiply( productA, productB );

		i++;
	}
	while( permutation.IsIdentity() );

	return true;
}

// PermutationStream.cpp