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
//                                PermutationProductStream
//------------------------------------------------------------------------------------------

// This could be used in conjunction with a stabilizer chain to enumerate
// all elements of a group without being limited by memory, only by running
// time.  It also has some other uses too in other contexts.
class PermutationProductStream : public PermutationStream
{
public:

	PermutationProductStream( void );
	virtual ~PermutationProductStream( void );

	virtual bool Reset( void ) override;
	virtual bool OutputPermutation( Permutation& permutation ) override;

	void Configure( const StabilizerChain* stabChain );
	void Clear( void );

	struct Component
	{
		uint offset;
		PermutationConstPtrArray* permutationArray;
	};

	typedef std::vector< Component > ComponentArray;
	ComponentArray componentArray;
	bool wrapped;
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
//                              PermutationFreeGroupStream
//------------------------------------------------------------------------------------------

// This may not actually generate a free group on N symbols exactly as such is defined,
// but it is an alternative to the PermutationWordStream that won't use up a ton of memory.
class PermutationFreeGroupStream : public PermutationStream
{
public:

	PermutationFreeGroupStream( const PermutationSet* generatorSet, const CompressInfo* compressInfo );
	virtual ~PermutationFreeGroupStream( void );

	virtual bool Reset( void ) override;
	virtual bool OutputPermutation( Permutation& permutation ) override;

	PermutationArray generatorArray;
	const CompressInfo* compressInfo;
	uint wordSize;
	PermutationProductStream* productStream;
};

//------------------------------------------------------------------------------------------
//                               PermutationWordStream
//------------------------------------------------------------------------------------------

class PermutationWordStream : public PermutationStream
{
public:

	PermutationWordStream( const PermutationSet* generatorSet, const CompressInfo* compressInfo );
	virtual ~PermutationWordStream( void );

	virtual bool Reset( void ) override;
	virtual bool OutputPermutation( Permutation& permutation ) override;

	const PermutationSet* generatorSet;
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

	PermutationConjugateStream( const PermutationSet* generatorSet, const CompressInfo* compressInfo );
	virtual ~PermutationConjugateStream( void );

	virtual bool Reset( void ) override;
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

	virtual bool Reset( void ) override;
	virtual bool OutputPermutation( Permutation& permutation ) override;

	PermutationArray permutationArray;
	uint i, j;
};

// PermutationStream.h