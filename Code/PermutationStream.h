// PermutationStream.h

#pragma once

#include "StabilizerChain.h"

//------------------------------------------------------------------------------------------
//                                 PermutationStream
//------------------------------------------------------------------------------------------

// These are just sources and destinations for permutations.  Many algorithms are written
// to consume or generate permutations, and so this provides a common interface for such things,
// and a way to connect different algorithms together, perhaps.
class PermutationStream
{
public:

	PermutationStream( void );
	virtual ~PermutationStream( void );

	virtual bool Reset( void );
	virtual bool OutputPermutation( Permutation& permutation );
	virtual bool InputPermutation( const Permutation& permutation );

	void LoadPermutationArray( PermutationArray& permutationArray, int loadMax = -1 );
	void UnloadPermutationArray( const PermutationArray& permuationArray );
};

//------------------------------------------------------------------------------------------
//								  PermutationFifoStream
//------------------------------------------------------------------------------------------

class PermutationFifoStream : public PermutationStream
{
public:

	PermutationFifoStream( void );
	virtual ~PermutationFifoStream( void );

	virtual bool OutputPermutation( Permutation& permutation ) override;
	virtual bool InputPermutation( const Permutation& permutation ) override;

	PermutationList permutationList;
};

//------------------------------------------------------------------------------------------
//                               PermutationStabGroupStream
//------------------------------------------------------------------------------------------

class PermutationStabGroupStream : public PermutationStream
{
public:

	PermutationStabGroupStream( StabilizerChain::Group* group );
	virtual ~PermutationStabGroupStream( void );

	StabilizerChain::Group* group;
};

//------------------------------------------------------------------------------------------
//                               PermutationWordStream
//------------------------------------------------------------------------------------------

class PermutationWordStream : public PermutationStabGroupStream
{
public:

	PermutationWordStream( StabilizerChain::Group* group, const CompressInfo* compressInfo );
	virtual ~PermutationWordStream( void );

	virtual bool Reset( void ) override;
	virtual bool OutputPermutation( Permutation& permutation ) override;

	const CompressInfo* compressInfo;
	PermutationSet processedSet;
	OrderedPermutationSet permutationQueue;
};

//------------------------------------------------------------------------------------------
//                                PermutationConjugateStream
//------------------------------------------------------------------------------------------

class PermutationConjugateStream : public PermutationWordStream
{
public:

	PermutationConjugateStream( StabilizerChain::Group* group, const CompressInfo* compressInfo );
	virtual ~PermutationConjugateStream( void );

	virtual bool OutputPermutation( Permutation& permutation ) override;

	PermutationArray permutationArray;
	Permutation conjugatingPermutation;
	Permutation invConjugatingPermutation;
	uint i;
};

//------------------------------------------------------------------------------------------
//                                PermutationCommutatorStream
//------------------------------------------------------------------------------------------

class PermutationCommutatorStream : public PermutationStream
{
public:

	PermutationCommutatorStream( void );
	virtual ~PermutationCommutatorStream( void );

	virtual bool OutputPermutation( Permutation& permutation ) override;

	PermutationArray permutationArray;
	uint i, j;
};

// PermutationStream.h