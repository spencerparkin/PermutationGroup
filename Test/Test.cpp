// Test.cpp

#include <iostream>
#include <fstream>
#include <time.h>
#include "StabilizerChain.h"
#include "PermutationStream.h"

enum Puzzle
{
	Bubbloid3x3x3,
	Rubiks2x2x2,
	Rubiks3x3x3,
	Rubiks2x3x3,
	Rubiks2x2x3,
	Rubiks3x3x3_LL,
	MixupCube,
	SymGrpMadPuzzle1,
	SymGrpMadPuzzle2,
	SymGrpMadPuzzle3,
	SymGrpMadPuzzle4,
	SymGrpMadPuzzle5,
	SymGrpMadPuzzle6,
	SymGrpMadPuzzle7,
	SymGroup,
};

const char* MakeGenerators( Puzzle puzzle, PermutationSet& generatorSet, UintArray& baseArray );

int main( int argc, char** argv )
{
	clock_t t = clock();

	StabilizerChain* stabChain = new StabilizerChain();
	Permutation permutation;
	UintArray baseArray;
	const char* fileName = nullptr;
	Puzzle puzzle = Rubiks3x3x3_LL;
	PermutationSet generatorSet;

	stabChain->logStream = &std::cout;

	fileName = MakeGenerators( puzzle, generatorSet, baseArray );

	bool success = stabChain->Generate( generatorSet, baseArray );
	if( !success )
		std::cout << "Failed!\n";

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	unsigned long long order = stabChain->GetSubGroupAtDepth(0)->Order();

	if( success )
	{
		unsigned long long order = stabChain->group->Order();

		stabChain->Print( std::cout );

		// Throw in the redundant generators at the root level.  This should help to reduce word size.
		//for( PermutationSet::iterator iter = generatorSet.begin(); iter != generatorSet.end(); iter++ )
		//	stabChain->group->generatorSet.insert( *iter );

		stabChain->group->NameGenerators();

		CompressInfo compressInfo;
		stabChain->group->MakeCompressInfo( compressInfo );

		const NaturalNumberSet& stabilizerSet = stabChain->group->GetSubgroupStabilizerPointSet();

		PermutationOrbitStream* permutationOrbitStream = new PermutationOrbitStream( &stabChain->group->generatorSet, *stabilizerSet.set.begin(), &compressInfo );

		PermutationWordStream* permutationWordStream = new PermutationWordStream( &stabChain->group->generatorSet, &compressInfo );
		permutationWordStream->queueMax = 100000;

		PermutationStabChainStream* permutationStabChainStream = new PermutationStabChainStream( stabChain, &compressInfo );

		PermutationMultiStream permutationMultiStream;
		permutationMultiStream.permutationStreamArray.push_back( permutationOrbitStream );
		permutationMultiStream.permutationStreamArray.push_back( permutationWordStream );
		permutationMultiStream.permutationStreamArray.push_back( permutationStabChainStream );

		if( stabChain->OptimizeNames( permutationMultiStream, compressInfo, 20.0 ) )
		{
			std::string jsonString;
			stabChain->SaveToJsonString( jsonString );
		
			std::fstream fstream;
			fstream.open( fileName, std::fstream::out );
			fstream << jsonString;
			fstream.close();
		}
	}

	getchar();

	delete stabChain;

	return 0;
}

const char* MakeGenerators( Puzzle puzzle, PermutationSet& generatorSet, UintArray& baseArray )
{
	const char* fileName = nullptr;

	switch( puzzle )
	{
		case SymGroup:
		{
			Permutation permutation;

			uint count = 8;

			for( uint i = 0; i < count - 1; i++ )
			{
				permutation.DefineIdentity();
				permutation.DefineCycle( i, i + 1 );
				generatorSet.insert( permutation );
			}

			for( uint i = 0; i < count; i++ )
				baseArray.push_back(i);

			break;
		}
		case SymGrpMadPuzzle1:
		{
			Permutation permutation;

			// R
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1, 2 );
			generatorSet.insert( permutation );

			// F0
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1 );
			generatorSet.insert( permutation );

			// F1
			permutation.DefineIdentity();
			permutation.DefineCycle( 1, 2 );
			generatorSet.insert( permutation );

			// F2
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 2 );
			generatorSet.insert( permutation );

			for( uint i = 0; i < 3; i++ )
				baseArray.push_back(i);

			fileName = "SymGrpMadPuzzle1.txt";

			break;
		}
		case SymGrpMadPuzzle2:
		{
			Permutation permutation;

			// R
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1, 2, 3 );
			generatorSet.insert( permutation );

			// Dp
			permutation.DefineIdentity();
			permutation.DefineCycle( 1, 3 );
			generatorSet.insert( permutation );

			// V
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1 );
			permutation.DefineCycle( 2, 3 );
			generatorSet.insert( permutation );

			// D
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 2 );
			generatorSet.insert( permutation );

			// H
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 3 );
			permutation.DefineCycle( 1, 2 );
			generatorSet.insert( permutation );

			for( uint i = 0; i < 4; i++ )
				baseArray.push_back(i);

			fileName = "SymGrpMadPuzzle2.txt";

			break;
		}
		case SymGrpMadPuzzle3:
		{
			Permutation permutation;

			// aR
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 4, 18 );
			permutation.DefineCycle( 1, 5, 19 );
			permutation.DefineCycle( 2, 6, 16 );
			permutation.DefineCycle( 3, 7, 17 );
			permutation.DefineCycle( 20, 21, 22 );
			generatorSet.insert( permutation );

			// aF0
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 4 );
			permutation.DefineCycle( 1, 7 );
			permutation.DefineCycle( 3, 5 );
			permutation.DefineCycle( 2, 6 );
			permutation.DefineCycle( 21, 22 );
			permutation.DefineCycle( 17, 19 );
			generatorSet.insert( permutation );

			// aF1
			permutation.DefineIdentity();
			permutation.DefineCycle( 4, 18 );
			permutation.DefineCycle( 5, 17 );
			permutation.DefineCycle( 7, 19 );
			permutation.DefineCycle( 6, 16 );
			permutation.DefineCycle( 20, 22 );
			permutation.DefineCycle( 1, 3 );
			generatorSet.insert( permutation );

			// aF2
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 18 );
			permutation.DefineCycle( 3, 19 );
			permutation.DefineCycle( 1, 17 );
			permutation.DefineCycle( 2, 16 );
			permutation.DefineCycle( 20, 21 );
			permutation.DefineCycle( 5, 7 );
			generatorSet.insert( permutation );

			// bR
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 12, 16 );
			permutation.DefineCycle( 9, 13, 17 );
			permutation.DefineCycle( 10, 14, 18 );
			permutation.DefineCycle( 11, 15, 19 );
			permutation.DefineCycle( 23, 24, 25 );
			generatorSet.insert( permutation );

			// bF0
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 16 );
			permutation.DefineCycle( 11, 17 );
			permutation.DefineCycle( 9, 19 );
			permutation.DefineCycle( 10, 18 );
			permutation.DefineCycle( 24, 25 );
			permutation.DefineCycle( 13, 15 );
			generatorSet.insert( permutation );

			// bF1
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 12 );
			permutation.DefineCycle( 9, 15 );
			permutation.DefineCycle( 11, 13 );
			permutation.DefineCycle( 10, 14 );
			permutation.DefineCycle( 23, 25 );
			permutation.DefineCycle( 17, 19 );
			generatorSet.insert( permutation );
			
			// bF2
			permutation.DefineIdentity();
			permutation.DefineCycle( 12, 16 );
			permutation.DefineCycle( 13, 19 );
			permutation.DefineCycle( 15, 17 );
			permutation.DefineCycle( 14, 18 );
			permutation.DefineCycle( 23, 24 );
			permutation.DefineCycle( 9, 11 );
			generatorSet.insert( permutation );

			for( uint i = 20; i < 26; i++ )
				baseArray.push_back(i);

			for( uint i = 0; i < 20; i++ )
				baseArray.push_back(i);

			fileName = "SymGrpMadPuzzle3.txt";

			break;
		}
		case SymGrpMadPuzzle4:
		{
			Permutation permutation;

			// bR
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 16, 19, 3 );
			permutation.DefineCycle( 1, 10, 18, 7 );
			permutation.DefineCycle( 2, 4, 17, 13 );
			permutation.DefineCycle( 5, 11, 12, 6 );
			generatorSet.insert( permutation );

			// R1
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 24, 27, 11 );
			permutation.DefineCycle( 9, 20, 26, 17 );
			permutation.DefineCycle( 10, 14, 25, 23 );
			permutation.DefineCycle( 15, 21, 22, 16 );
			generatorSet.insert( permutation );

			// V0
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 3 );
			permutation.DefineCycle( 1, 2 );
			permutation.DefineCycle( 4, 7 );
			permutation.DefineCycle( 5, 6 );
			permutation.DefineCycle( 10, 13 );
			permutation.DefineCycle( 11, 12 );
			permutation.DefineCycle( 16, 19 );
			permutation.DefineCycle( 17, 18 );
			generatorSet.insert( permutation );

			// H0
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 16 );
			permutation.DefineCycle( 4, 10 );
			permutation.DefineCycle( 1, 17 );
			permutation.DefineCycle( 5, 11 );
			permutation.DefineCycle( 2, 18 );
			permutation.DefineCycle( 6, 12 );
			permutation.DefineCycle( 3, 19 );
			permutation.DefineCycle( 7, 13 );
			generatorSet.insert( permutation );

			// V1
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 11 );
			permutation.DefineCycle( 9, 10 );
			permutation.DefineCycle( 14, 17 );
			permutation.DefineCycle( 15, 16 );
			permutation.DefineCycle( 20, 23 );
			permutation.DefineCycle( 21, 22 );
			permutation.DefineCycle( 24, 27 );
			permutation.DefineCycle( 25, 26 );
			generatorSet.insert( permutation );

			// H1
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 24 );
			permutation.DefineCycle( 14, 20 );
			permutation.DefineCycle( 9, 25 );
			permutation.DefineCycle( 15, 21 );
			permutation.DefineCycle( 10, 26 );
			permutation.DefineCycle( 16, 22 );
			permutation.DefineCycle( 11, 27 );
			permutation.DefineCycle( 17, 23 );
			generatorSet.insert( permutation );

			// D0
			permutation.DefineIdentity();
			permutation.DefineCycle( 1, 4 );
			permutation.DefineCycle( 2, 10 );
			permutation.DefineCycle( 3, 16 );
			permutation.DefineCycle( 6, 11 );
			permutation.DefineCycle( 7, 17 );
			permutation.DefineCycle( 13, 18 );
			generatorSet.insert( permutation );

			// D0'
			permutation.DefineIdentity();
			permutation.DefineCycle( 2, 7 );
			permutation.DefineCycle( 1, 13 );
			permutation.DefineCycle( 0, 19 );
			permutation.DefineCycle( 5, 12 );
			permutation.DefineCycle( 4, 18 );
			permutation.DefineCycle( 10, 17 );
			generatorSet.insert( permutation );

			// D1
			permutation.DefineIdentity();
			permutation.DefineCycle( 9, 14 );
			permutation.DefineCycle( 10, 20 );
			permutation.DefineCycle( 11, 24 );
			permutation.DefineCycle( 16, 21 );
			permutation.DefineCycle( 17, 25 );
			permutation.DefineCycle( 23, 26 );
			generatorSet.insert( permutation );

			// D1'
			permutation.DefineIdentity();
			permutation.DefineCycle( 10, 17 );
			permutation.DefineCycle( 9, 23 );
			permutation.DefineCycle( 8, 27 );
			permutation.DefineCycle( 15, 22 );
			permutation.DefineCycle( 14, 26 );
			permutation.DefineCycle( 20, 25 );
			generatorSet.insert( permutation );

			for( uint i = 0; i < 28; i++ )
				baseArray.push_back(i);

			fileName = "SymGrpMadPuzzle4.txt";

			break;
		}
		case SymGrpMadPuzzle5:
		{
			Permutation permR0;
			permR0.Define( 0, 46 );
			permR0.Define( 1, 36 );
			permR0.Define( 2, 26 );
			permR0.Define( 3, 16 );
			permR0.Define( 4, 6 );
			permR0.Define( 5, 0 );
			permR0.Define( 6, 47 );
			permR0.Define( 7, 37 );
			permR0.Define( 8, 27 );
			permR0.Define( 9, 17 );
			permR0.Define( 10, 7 );
			permR0.Define( 11, 1 );
			permR0.Define( 16, 48 );
			permR0.Define( 17, 38 );
			permR0.Define( 18, 28 );
			permR0.Define( 19, 18 );
			permR0.Define( 20, 8 );
			permR0.Define( 21, 2 );
			permR0.Define( 26, 49 );
			permR0.Define( 27, 39 );
			permR0.Define( 28, 29 );
			permR0.Define( 29, 19 );
			permR0.Define( 30, 9 );
			permR0.Define( 31, 3 );
			permR0.Define( 36, 50 );
			permR0.Define( 37, 40 );
			permR0.Define( 38, 30 );
			permR0.Define( 39, 20 );
			permR0.Define( 40, 10 );
			permR0.Define( 41, 4 );
			permR0.Define( 46, 51 );
			permR0.Define( 47, 41 );
			permR0.Define( 48, 31 );
			permR0.Define( 49, 21 );
			permR0.Define( 50, 11 );
			permR0.Define( 51, 5 );
			generatorSet.insert( permR0 );

			Permutation permD0p;
			permD0p.Define( 0, 51 );
			permD0p.Define( 1, 41 );
			permD0p.Define( 2, 31 );
			permD0p.Define( 3, 21 );
			permD0p.Define( 4, 11 );
			permD0p.Define( 6, 50 );
			permD0p.Define( 7, 40 );
			permD0p.Define( 8, 30 );
			permD0p.Define( 9, 20 );
			permD0p.Define( 11, 4 );
			permD0p.Define( 16, 49 );
			permD0p.Define( 17, 39 );
			permD0p.Define( 18, 29 );
			permD0p.Define( 20, 9 );
			permD0p.Define( 21, 3 );
			permD0p.Define( 26, 48 );
			permD0p.Define( 27, 38 );
			permD0p.Define( 29, 18 );
			permD0p.Define( 30, 8 );
			permD0p.Define( 31, 2 );
			permD0p.Define( 36, 47 );
			permD0p.Define( 38, 27 );
			permD0p.Define( 39, 17 );
			permD0p.Define( 40, 7 );
			permD0p.Define( 41, 1 );
			permD0p.Define( 47, 36 );
			permD0p.Define( 48, 26 );
			permD0p.Define( 49, 16 );
			permD0p.Define( 50, 6 );
			permD0p.Define( 51, 0 );
			generatorSet.insert( permD0p );

			Permutation permV0;
			permV0.Define( 0, 5 );
			permV0.Define( 1, 4 );
			permV0.Define( 2, 3 );
			permV0.Define( 3, 2 );
			permV0.Define( 4, 1 );
			permV0.Define( 5, 0 );
			permV0.Define( 6, 11 );
			permV0.Define( 7, 10 );
			permV0.Define( 8, 9 );
			permV0.Define( 9, 8 );
			permV0.Define( 10, 7 );
			permV0.Define( 11, 6 );
			permV0.Define( 16, 21 );
			permV0.Define( 17, 20 );
			permV0.Define( 18, 19 );
			permV0.Define( 19, 18 );
			permV0.Define( 20, 17 );
			permV0.Define( 21, 16 );
			permV0.Define( 26, 31 );
			permV0.Define( 27, 30 );
			permV0.Define( 28, 29 );
			permV0.Define( 29, 28 );
			permV0.Define( 30, 27 );
			permV0.Define( 31, 26 );
			permV0.Define( 36, 41 );
			permV0.Define( 37, 40 );
			permV0.Define( 38, 39 );
			permV0.Define( 39, 38 );
			permV0.Define( 40, 37 );
			permV0.Define( 41, 36 );
			permV0.Define( 46, 51 );
			permV0.Define( 47, 50 );
			permV0.Define( 48, 49 );
			permV0.Define( 49, 48 );
			permV0.Define( 50, 47 );
			permV0.Define( 51, 46 );
			generatorSet.insert( permV0 );

			Permutation permD0;
			permD0.Define( 1, 6 );
			permD0.Define( 2, 16 );
			permD0.Define( 3, 26 );
			permD0.Define( 4, 36 );
			permD0.Define( 5, 46 );
			permD0.Define( 6, 1 );
			permD0.Define( 8, 17 );
			permD0.Define( 9, 27 );
			permD0.Define( 10, 37 );
			permD0.Define( 11, 47 );
			permD0.Define( 16, 2 );
			permD0.Define( 17, 8 );
			permD0.Define( 19, 28 );
			permD0.Define( 20, 38 );
			permD0.Define( 21, 48 );
			permD0.Define( 26, 3 );
			permD0.Define( 27, 9 );
			permD0.Define( 28, 19 );
			permD0.Define( 30, 39 );
			permD0.Define( 31, 49 );
			permD0.Define( 36, 4 );
			permD0.Define( 37, 10 );
			permD0.Define( 38, 20 );
			permD0.Define( 39, 30 );
			permD0.Define( 41, 50 );
			permD0.Define( 46, 5 );
			permD0.Define( 47, 11 );
			permD0.Define( 48, 21 );
			permD0.Define( 49, 31 );
			permD0.Define( 50, 41 );
			generatorSet.insert( permD0 );

			Permutation permH0;
			permH0.Define( 0, 46 );
			permH0.Define( 1, 47 );
			permH0.Define( 2, 48 );
			permH0.Define( 3, 49 );
			permH0.Define( 4, 50 );
			permH0.Define( 5, 51 );
			permH0.Define( 6, 36 );
			permH0.Define( 7, 37 );
			permH0.Define( 8, 38 );
			permH0.Define( 9, 39 );
			permH0.Define( 10, 40 );
			permH0.Define( 11, 41 );
			permH0.Define( 16, 26 );
			permH0.Define( 17, 27 );
			permH0.Define( 18, 28 );
			permH0.Define( 19, 29 );
			permH0.Define( 20, 30 );
			permH0.Define( 21, 31 );
			permH0.Define( 26, 16 );
			permH0.Define( 27, 17 );
			permH0.Define( 28, 18 );
			permH0.Define( 29, 19 );
			permH0.Define( 30, 20 );
			permH0.Define( 31, 21 );
			permH0.Define( 36, 6 );
			permH0.Define( 37, 7 );
			permH0.Define( 38, 8 );
			permH0.Define( 39, 9 );
			permH0.Define( 40, 10 );
			permH0.Define( 41, 11 );
			permH0.Define( 46, 0 );
			permH0.Define( 47, 1 );
			permH0.Define( 48, 2 );
			permH0.Define( 49, 3 );
			permH0.Define( 50, 4 );
			permH0.Define( 51, 5 );
			generatorSet.insert( permH0 );

			Permutation permR1;
			permR1.Define( 12, 58 );
			permR1.Define( 13, 52 );
			permR1.Define( 14, 42 );
			permR1.Define( 15, 32 );
			permR1.Define( 16, 22 );
			permR1.Define( 17, 12 );
			permR1.Define( 22, 59 );
			permR1.Define( 23, 53 );
			permR1.Define( 24, 43 );
			permR1.Define( 25, 33 );
			permR1.Define( 26, 23 );
			permR1.Define( 27, 13 );
			permR1.Define( 32, 60 );
			permR1.Define( 33, 54 );
			permR1.Define( 34, 44 );
			permR1.Define( 35, 34 );
			permR1.Define( 36, 24 );
			permR1.Define( 37, 14 );
			permR1.Define( 42, 61 );
			permR1.Define( 43, 55 );
			permR1.Define( 44, 45 );
			permR1.Define( 45, 35 );
			permR1.Define( 46, 25 );
			permR1.Define( 47, 15 );
			permR1.Define( 52, 62 );
			permR1.Define( 53, 56 );
			permR1.Define( 54, 46 );
			permR1.Define( 55, 36 );
			permR1.Define( 56, 26 );
			permR1.Define( 57, 16 );
			permR1.Define( 58, 63 );
			permR1.Define( 59, 57 );
			permR1.Define( 60, 47 );
			permR1.Define( 61, 37 );
			permR1.Define( 62, 27 );
			permR1.Define( 63, 17 );
			generatorSet.insert( permR1 );

			Permutation permD1p;
			permD1p.Define( 12, 63 );
			permD1p.Define( 13, 57 );
			permD1p.Define( 14, 47 );
			permD1p.Define( 15, 37 );
			permD1p.Define( 16, 27 );
			permD1p.Define( 22, 62 );
			permD1p.Define( 23, 56 );
			permD1p.Define( 24, 46 );
			permD1p.Define( 25, 36 );
			permD1p.Define( 27, 16 );
			permD1p.Define( 32, 61 );
			permD1p.Define( 33, 55 );
			permD1p.Define( 34, 45 );
			permD1p.Define( 36, 25 );
			permD1p.Define( 37, 15 );
			permD1p.Define( 42, 60 );
			permD1p.Define( 43, 54 );
			permD1p.Define( 45, 34 );
			permD1p.Define( 46, 24 );
			permD1p.Define( 47, 14 );
			permD1p.Define( 52, 59 );
			permD1p.Define( 54, 43 );
			permD1p.Define( 55, 33 );
			permD1p.Define( 56, 23 );
			permD1p.Define( 57, 13 );
			permD1p.Define( 59, 52 );
			permD1p.Define( 60, 42 );
			permD1p.Define( 61, 32 );
			permD1p.Define( 62, 22 );
			permD1p.Define( 63, 12 );
			generatorSet.insert( permD1p );

			Permutation permV1;
			permV1.Define( 12, 17 );
			permV1.Define( 13, 16 );
			permV1.Define( 14, 15 );
			permV1.Define( 15, 14 );
			permV1.Define( 16, 13 );
			permV1.Define( 17, 12 );
			permV1.Define( 22, 27 );
			permV1.Define( 23, 26 );
			permV1.Define( 24, 25 );
			permV1.Define( 25, 24 );
			permV1.Define( 26, 23 );
			permV1.Define( 27, 22 );
			permV1.Define( 32, 37 );
			permV1.Define( 33, 36 );
			permV1.Define( 34, 35 );
			permV1.Define( 35, 34 );
			permV1.Define( 36, 33 );
			permV1.Define( 37, 32 );
			permV1.Define( 42, 47 );
			permV1.Define( 43, 46 );
			permV1.Define( 44, 45 );
			permV1.Define( 45, 44 );
			permV1.Define( 46, 43 );
			permV1.Define( 47, 42 );
			permV1.Define( 52, 57 );
			permV1.Define( 53, 56 );
			permV1.Define( 54, 55 );
			permV1.Define( 55, 54 );
			permV1.Define( 56, 53 );
			permV1.Define( 57, 52 );
			permV1.Define( 58, 63 );
			permV1.Define( 59, 62 );
			permV1.Define( 60, 61 );
			permV1.Define( 61, 60 );
			permV1.Define( 62, 59 );
			permV1.Define( 63, 58 );
			generatorSet.insert( permV1 );

			Permutation permD1;
			permD1.Define( 13, 22 );
			permD1.Define( 14, 32 );
			permD1.Define( 15, 42 );
			permD1.Define( 16, 52 );
			permD1.Define( 17, 58 );
			permD1.Define( 22, 13 );
			permD1.Define( 24, 33 );
			permD1.Define( 25, 43 );
			permD1.Define( 26, 53 );
			permD1.Define( 27, 59 );
			permD1.Define( 32, 14 );
			permD1.Define( 33, 24 );
			permD1.Define( 35, 44 );
			permD1.Define( 36, 54 );
			permD1.Define( 37, 60 );
			permD1.Define( 42, 15 );
			permD1.Define( 43, 25 );
			permD1.Define( 44, 35 );
			permD1.Define( 46, 55 );
			permD1.Define( 47, 61 );
			permD1.Define( 52, 16 );
			permD1.Define( 53, 26 );
			permD1.Define( 54, 36 );
			permD1.Define( 55, 46 );
			permD1.Define( 57, 62 );
			permD1.Define( 58, 17 );
			permD1.Define( 59, 27 );
			permD1.Define( 60, 37 );
			permD1.Define( 61, 47 );
			permD1.Define( 62, 57 );
			generatorSet.insert( permD1 );

			Permutation permH1;
			permH1.Define( 12, 58 );
			permH1.Define( 13, 59 );
			permH1.Define( 14, 60 );
			permH1.Define( 15, 61 );
			permH1.Define( 16, 62 );
			permH1.Define( 17, 63 );
			permH1.Define( 22, 52 );
			permH1.Define( 23, 53 );
			permH1.Define( 24, 54 );
			permH1.Define( 25, 55 );
			permH1.Define( 26, 56 );
			permH1.Define( 27, 57 );
			permH1.Define( 32, 42 );
			permH1.Define( 33, 43 );
			permH1.Define( 34, 44 );
			permH1.Define( 35, 45 );
			permH1.Define( 36, 46 );
			permH1.Define( 37, 47 );
			permH1.Define( 42, 32 );
			permH1.Define( 43, 33 );
			permH1.Define( 44, 34 );
			permH1.Define( 45, 35 );
			permH1.Define( 46, 36 );
			permH1.Define( 47, 37 );
			permH1.Define( 52, 22 );
			permH1.Define( 53, 23 );
			permH1.Define( 54, 24 );
			permH1.Define( 55, 25 );
			permH1.Define( 56, 26 );
			permH1.Define( 57, 27 );
			permH1.Define( 58, 12 );
			permH1.Define( 59, 13 );
			permH1.Define( 60, 14 );
			permH1.Define( 61, 15 );
			permH1.Define( 62, 16 );
			permH1.Define( 63, 17 );
			generatorSet.insert( permH1 );

			for( uint i = 0; i < 64; i++ )
				baseArray.push_back(i);

			fileName = "SymGrpMadPuzzle5.txt";

			break;
		}
		case SymGrpMadPuzzle6:
		{
			Permutation permutation;

			// aR
			permutation.DefineIdentity();
			permutation.DefineCycle( 25, 26 );
			permutation.DefineCycle( 20, 31 );
			permutation.DefineCycle( 24, 27 );
			permutation.DefineCycle( 21, 30 );
			permutation.DefineCycle( 22, 29 );
			permutation.DefineCycle( 23, 28 );
			generatorSet.insert( permutation );

			// aH
			permutation.DefineIdentity();
			permutation.DefineCycle( 20, 26 );
			permutation.DefineCycle( 21, 27 );
			permutation.DefineCycle( 22, 28 );
			permutation.DefineCycle( 23, 29 );
			permutation.DefineCycle( 24, 30 );
			permutation.DefineCycle( 25, 31 );
			generatorSet.insert( permutation );

			// aV
			permutation.DefineIdentity();
			permutation.DefineCycle( 20, 25 );
			permutation.DefineCycle( 21, 24 );
			permutation.DefineCycle( 22, 23 );
			permutation.DefineCycle( 26, 31 );
			permutation.DefineCycle( 27, 30 );
			permutation.DefineCycle( 28, 29 );
			generatorSet.insert( permutation );

			// bR
			permutation.DefineIdentity();
			permutation.DefineCycle( 5, 30 );
			permutation.DefineCycle( 4, 31 );
			permutation.DefineCycle( 10, 25 );
			permutation.DefineCycle( 11, 24 );
			permutation.DefineCycle( 14, 19 );
			permutation.DefineCycle( 15, 18 );
			generatorSet.insert( permutation );

			// bH
			permutation.DefineIdentity();
			permutation.DefineCycle( 4, 30 );
			permutation.DefineCycle( 10, 24 );
			permutation.DefineCycle( 14, 18 );
			permutation.DefineCycle( 5, 31 );
			permutation.DefineCycle( 11, 25 );
			permutation.DefineCycle( 15, 19 );
			generatorSet.insert( permutation );

			// bV
			permutation.DefineIdentity();
			permutation.DefineCycle( 4, 5 );
			permutation.DefineCycle( 10, 11 );
			permutation.DefineCycle( 14, 15 );
			permutation.DefineCycle( 18, 19 );
			permutation.DefineCycle( 24, 25 );
			permutation.DefineCycle( 30, 31 );
			generatorSet.insert( permutation );

			// cR
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 11 );
			permutation.DefineCycle( 6, 5 );
			permutation.DefineCycle( 1, 10 );
			permutation.DefineCycle( 4, 7 );
			permutation.DefineCycle( 2, 9 );
			permutation.DefineCycle( 3, 8 );
			generatorSet.insert( permutation );

			// cH
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 6 );
			permutation.DefineCycle( 1, 7 );
			permutation.DefineCycle( 2, 8 );
			permutation.DefineCycle( 3, 9 );
			permutation.DefineCycle( 4, 10 );
			permutation.DefineCycle( 5, 11 );
			generatorSet.insert( permutation );

			// cV
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 5 );
			permutation.DefineCycle( 1, 4 );
			permutation.DefineCycle( 2, 3 );
			permutation.DefineCycle( 6, 11 );
			permutation.DefineCycle( 7, 10 );
			permutation.DefineCycle( 8, 9 );
			generatorSet.insert( permutation );

			// dR
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 27 );
			permutation.DefineCycle( 1, 26 );
			permutation.DefineCycle( 6, 21 );
			permutation.DefineCycle( 7, 20 );
			permutation.DefineCycle( 12, 17 );
			permutation.DefineCycle( 13, 16 );
			generatorSet.insert( permutation );

			// dH
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 26 );
			permutation.DefineCycle( 6, 20 );
			permutation.DefineCycle( 12, 16 );
			permutation.DefineCycle( 1, 27 );
			permutation.DefineCycle( 7, 21 );
			permutation.DefineCycle( 13, 17 );
			generatorSet.insert( permutation );

			// dV
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1 );
			permutation.DefineCycle( 6, 7 );
			permutation.DefineCycle( 12, 13 );
			permutation.DefineCycle( 16, 17 );
			permutation.DefineCycle( 20, 21 );
			permutation.DefineCycle( 26, 27 );
			generatorSet.insert( permutation );

			for( uint i = 0; i < 32; i++ )
				baseArray.push_back(i);

			fileName = "SymGrpMadPuzzle6.txt";

			break;
		}
		case SymGrpMadPuzzle7:
		{
			Permutation R_0;
			R_0.Define( 0, 25 );
			R_0.Define( 9, 22 );
			R_0.Define( 18, 19 );
			R_0.Define( 19, 0 );
			R_0.Define( 20, 18 );
			R_0.Define( 21, 23 );
			R_0.Define( 22, 24 );
			R_0.Define( 23, 9 );
			R_0.Define( 24, 21 );
			R_0.Define( 25, 20 );
			R_0.Define( 26, 43 );
			R_0.Define( 35, 40 );
			R_0.Define( 36, 37 );
			R_0.Define( 37, 26 );
			R_0.Define( 38, 36 );
			R_0.Define( 39, 41 );
			R_0.Define( 40, 42 );
			R_0.Define( 41, 35 );
			R_0.Define( 42, 39 );
			R_0.Define( 43, 38 );
			R_0.Define( 44, 48 );
			R_0.Define( 45, 46 );
			R_0.Define( 46, 47 );
			R_0.Define( 47, 44 );
			R_0.Define( 48, 45 );
			generatorSet.insert( R_0 );

			Permutation F0_0;
			F0_0.Define( 0, 24 );
			F0_0.Define( 9, 20 );
			F0_0.Define( 18, 23 );
			F0_0.Define( 19, 21 );
			F0_0.Define( 20, 9 );
			F0_0.Define( 21, 19 );
			F0_0.Define( 22, 25 );
			F0_0.Define( 23, 18 );
			F0_0.Define( 24, 0 );
			F0_0.Define( 25, 22 );
			F0_0.Define( 26, 42 );
			F0_0.Define( 35, 38 );
			F0_0.Define( 36, 41 );
			F0_0.Define( 37, 39 );
			F0_0.Define( 38, 35 );
			F0_0.Define( 39, 37 );
			F0_0.Define( 40, 43 );
			F0_0.Define( 41, 36 );
			F0_0.Define( 42, 26 );
			F0_0.Define( 43, 40 );
			F0_0.Define( 44, 47 );
			F0_0.Define( 46, 48 );
			F0_0.Define( 47, 44 );
			F0_0.Define( 48, 46 );
			generatorSet.insert( F0_0 );

			Permutation F1_0;
			F1_0.Define( 0, 23 );
			F1_0.Define( 9, 19 );
			F1_0.Define( 18, 22 );
			F1_0.Define( 19, 9 );
			F1_0.Define( 20, 24 );
			F1_0.Define( 21, 25 );
			F1_0.Define( 22, 18 );
			F1_0.Define( 23, 0 );
			F1_0.Define( 24, 20 );
			F1_0.Define( 25, 21 );
			F1_0.Define( 26, 41 );
			F1_0.Define( 35, 37 );
			F1_0.Define( 36, 40 );
			F1_0.Define( 37, 35 );
			F1_0.Define( 38, 42 );
			F1_0.Define( 39, 43 );
			F1_0.Define( 40, 36 );
			F1_0.Define( 41, 26 );
			F1_0.Define( 42, 38 );
			F1_0.Define( 43, 39 );
			F1_0.Define( 44, 48 );
			F1_0.Define( 45, 47 );
			F1_0.Define( 47, 45 );
			F1_0.Define( 48, 44 );
			generatorSet.insert( F1_0 );

			Permutation F2_0;
			F2_0.Define( 0, 22 );
			F2_0.Define( 9, 25 );
			F2_0.Define( 18, 21 );
			F2_0.Define( 19, 24 );
			F2_0.Define( 20, 23 );
			F2_0.Define( 21, 18 );
			F2_0.Define( 22, 0 );
			F2_0.Define( 23, 20 );
			F2_0.Define( 24, 19 );
			F2_0.Define( 25, 9 );
			F2_0.Define( 26, 40 );
			F2_0.Define( 35, 43 );
			F2_0.Define( 36, 39 );
			F2_0.Define( 37, 42 );
			F2_0.Define( 38, 41 );
			F2_0.Define( 39, 36 );
			F2_0.Define( 40, 26 );
			F2_0.Define( 41, 38 );
			F2_0.Define( 42, 37 );
			F2_0.Define( 43, 35 );
			F2_0.Define( 44, 46 );
			F2_0.Define( 45, 48 );
			F2_0.Define( 46, 44 );
			F2_0.Define( 48, 45 );
			generatorSet.insert( F2_0 );

			Permutation F3_0;
			F3_0.Define( 0, 21 );
			F3_0.Define( 9, 18 );
			F3_0.Define( 18, 9 );
			F3_0.Define( 19, 23 );
			F3_0.Define( 20, 22 );
			F3_0.Define( 21, 0 );
			F3_0.Define( 22, 20 );
			F3_0.Define( 23, 19 );
			F3_0.Define( 24, 25 );
			F3_0.Define( 25, 24 );
			F3_0.Define( 26, 39 );
			F3_0.Define( 35, 36 );
			F3_0.Define( 36, 35 );
			F3_0.Define( 37, 41 );
			F3_0.Define( 38, 40 );
			F3_0.Define( 39, 26 );
			F3_0.Define( 40, 38 );
			F3_0.Define( 41, 37 );
			F3_0.Define( 42, 43 );
			F3_0.Define( 43, 42 );
			F3_0.Define( 45, 46 );
			F3_0.Define( 46, 45 );
			F3_0.Define( 47, 48 );
			F3_0.Define( 48, 47 );
			generatorSet.insert( F3_0 );

			Permutation F4_0;
			F4_0.Define( 0, 9 );
			F4_0.Define( 9, 0 );
			F4_0.Define( 18, 24 );
			F4_0.Define( 19, 22 );
			F4_0.Define( 20, 21 );
			F4_0.Define( 21, 20 );
			F4_0.Define( 22, 19 );
			F4_0.Define( 23, 25 );
			F4_0.Define( 24, 18 );
			F4_0.Define( 25, 23 );
			F4_0.Define( 26, 35 );
			F4_0.Define( 35, 26 );
			F4_0.Define( 36, 42 );
			F4_0.Define( 37, 40 );
			F4_0.Define( 38, 39 );
			F4_0.Define( 39, 38 );
			F4_0.Define( 40, 37 );
			F4_0.Define( 41, 43 );
			F4_0.Define( 42, 36 );
			F4_0.Define( 43, 41 );
			F4_0.Define( 44, 45 );
			F4_0.Define( 45, 44 );
			F4_0.Define( 46, 47 );
			F4_0.Define( 47, 46 );
			generatorSet.insert( F4_0 );

			Permutation R_1;
			R_1.Define( 9, 17 );
			R_1.Define( 10, 15 );
			R_1.Define( 11, 12 );
			R_1.Define( 12, 13 );
			R_1.Define( 13, 9 );
			R_1.Define( 14, 16 );
			R_1.Define( 15, 19 );
			R_1.Define( 16, 10 );
			R_1.Define( 17, 11 );
			R_1.Define( 19, 14 );
			R_1.Define( 26, 34 );
			R_1.Define( 27, 32 );
			R_1.Define( 28, 29 );
			R_1.Define( 29, 30 );
			R_1.Define( 30, 26 );
			R_1.Define( 31, 33 );
			R_1.Define( 32, 41 );
			R_1.Define( 33, 27 );
			R_1.Define( 34, 28 );
			R_1.Define( 41, 31 );
			R_1.Define( 49, 53 );
			R_1.Define( 50, 51 );
			R_1.Define( 51, 52 );
			R_1.Define( 52, 49 );
			R_1.Define( 53, 50 );
			generatorSet.insert( R_1 );

			Permutation F0_1;
			F0_1.Define( 9, 16 );
			F0_1.Define( 10, 13 );
			F0_1.Define( 11, 19 );
			F0_1.Define( 12, 15 );
			F0_1.Define( 13, 10 );
			F0_1.Define( 14, 17 );
			F0_1.Define( 15, 12 );
			F0_1.Define( 16, 9 );
			F0_1.Define( 17, 14 );
			F0_1.Define( 19, 11 );
			F0_1.Define( 26, 33 );
			F0_1.Define( 27, 30 );
			F0_1.Define( 28, 41 );
			F0_1.Define( 29, 32 );
			F0_1.Define( 30, 27 );
			F0_1.Define( 31, 34 );
			F0_1.Define( 32, 29 );
			F0_1.Define( 33, 26 );
			F0_1.Define( 34, 31 );
			F0_1.Define( 41, 28 );
			F0_1.Define( 49, 52 );
			F0_1.Define( 51, 53 );
			F0_1.Define( 52, 49 );
			F0_1.Define( 53, 51 );
			generatorSet.insert( F0_1 );

			Permutation F1_1;
			F1_1.Define( 9, 15 );
			F1_1.Define( 10, 17 );
			F1_1.Define( 11, 16 );
			F1_1.Define( 12, 14 );
			F1_1.Define( 13, 19 );
			F1_1.Define( 14, 12 );
			F1_1.Define( 15, 9 );
			F1_1.Define( 16, 11 );
			F1_1.Define( 17, 10 );
			F1_1.Define( 19, 13 );
			F1_1.Define( 26, 32 );
			F1_1.Define( 27, 34 );
			F1_1.Define( 28, 33 );
			F1_1.Define( 29, 31 );
			F1_1.Define( 30, 41 );
			F1_1.Define( 31, 29 );
			F1_1.Define( 32, 26 );
			F1_1.Define( 33, 28 );
			F1_1.Define( 34, 27 );
			F1_1.Define( 41, 30 );
			F1_1.Define( 49, 53 );
			F1_1.Define( 50, 52 );
			F1_1.Define( 52, 50 );
			F1_1.Define( 53, 49 );
			generatorSet.insert( F1_1 );

			Permutation F2_1;
			F2_1.Define( 9, 14 );
			F2_1.Define( 10, 12 );
			F2_1.Define( 11, 15 );
			F2_1.Define( 12, 10 );
			F2_1.Define( 13, 16 );
			F2_1.Define( 14, 9 );
			F2_1.Define( 15, 11 );
			F2_1.Define( 16, 13 );
			F2_1.Define( 17, 19 );
			F2_1.Define( 19, 17 );
			F2_1.Define( 26, 31 );
			F2_1.Define( 27, 29 );
			F2_1.Define( 28, 32 );
			F2_1.Define( 29, 27 );
			F2_1.Define( 30, 33 );
			F2_1.Define( 31, 26 );
			F2_1.Define( 32, 28 );
			F2_1.Define( 33, 30 );
			F2_1.Define( 34, 41 );
			F2_1.Define( 41, 34 );
			F2_1.Define( 49, 51 );
			F2_1.Define( 50, 53 );
			F2_1.Define( 51, 49 );
			F2_1.Define( 53, 50 );
			generatorSet.insert( F2_1 );

			Permutation F3_1;
			F3_1.Define( 9, 10 );
			F3_1.Define( 10, 9 );
			F3_1.Define( 11, 14 );
			F3_1.Define( 12, 19 );
			F3_1.Define( 13, 15 );
			F3_1.Define( 14, 11 );
			F3_1.Define( 15, 13 );
			F3_1.Define( 16, 17 );
			F3_1.Define( 17, 16 );
			F3_1.Define( 19, 12 );
			F3_1.Define( 26, 27 );
			F3_1.Define( 27, 26 );
			F3_1.Define( 28, 31 );
			F3_1.Define( 29, 41 );
			F3_1.Define( 30, 32 );
			F3_1.Define( 31, 28 );
			F3_1.Define( 32, 30 );
			F3_1.Define( 33, 34 );
			F3_1.Define( 34, 33 );
			F3_1.Define( 41, 29 );
			F3_1.Define( 50, 51 );
			F3_1.Define( 51, 50 );
			F3_1.Define( 52, 53 );
			F3_1.Define( 53, 52 );
			generatorSet.insert( F3_1 );

			Permutation F4_1;
			F4_1.Define( 9, 19 );
			F4_1.Define( 10, 11 );
			F4_1.Define( 11, 10 );
			F4_1.Define( 12, 16 );
			F4_1.Define( 13, 14 );
			F4_1.Define( 14, 13 );
			F4_1.Define( 15, 17 );
			F4_1.Define( 16, 12 );
			F4_1.Define( 17, 15 );
			F4_1.Define( 19, 9 );
			F4_1.Define( 26, 41 );
			F4_1.Define( 27, 28 );
			F4_1.Define( 28, 27 );
			F4_1.Define( 29, 33 );
			F4_1.Define( 30, 31 );
			F4_1.Define( 31, 30 );
			F4_1.Define( 32, 34 );
			F4_1.Define( 33, 29 );
			F4_1.Define( 34, 32 );
			F4_1.Define( 41, 26 );
			F4_1.Define( 49, 50 );
			F4_1.Define( 50, 49 );
			F4_1.Define( 51, 52 );
			F4_1.Define( 52, 51 );
			generatorSet.insert( F4_1 );

			Permutation R_2;
			R_2.Define( 0, 8 );
			R_2.Define( 1, 6 );
			R_2.Define( 2, 3 );
			R_2.Define( 3, 0 );
			R_2.Define( 4, 2 );
			R_2.Define( 5, 7 );
			R_2.Define( 6, 22 );
			R_2.Define( 7, 1 );
			R_2.Define( 8, 4 );
			R_2.Define( 22, 5 );
			R_2.Define( 54, 58 );
			R_2.Define( 55, 56 );
			R_2.Define( 56, 57 );
			R_2.Define( 57, 54 );
			R_2.Define( 58, 55 );
			generatorSet.insert( R_2 );

			Permutation F0_2;
			F0_2.Define( 0, 22 );
			F0_2.Define( 1, 4 );
			F0_2.Define( 2, 7 );
			F0_2.Define( 3, 5 );
			F0_2.Define( 4, 1 );
			F0_2.Define( 5, 3 );
			F0_2.Define( 6, 8 );
			F0_2.Define( 7, 2 );
			F0_2.Define( 8, 6 );
			F0_2.Define( 22, 0 );
			F0_2.Define( 54, 57 );
			F0_2.Define( 56, 58 );
			F0_2.Define( 57, 54 );
			F0_2.Define( 58, 56 );
			generatorSet.insert( F0_2 );

			Permutation F1_2;
			F1_2.Define( 0, 7 );
			F1_2.Define( 1, 3 );
			F1_2.Define( 2, 6 );
			F1_2.Define( 3, 1 );
			F1_2.Define( 4, 22 );
			F1_2.Define( 5, 8 );
			F1_2.Define( 6, 2 );
			F1_2.Define( 7, 0 );
			F1_2.Define( 8, 5 );
			F1_2.Define( 22, 4 );
			F1_2.Define( 54, 58 );
			F1_2.Define( 55, 57 );
			F1_2.Define( 57, 55 );
			F1_2.Define( 58, 54 );
			generatorSet.insert( F1_2 );

			Permutation F2_2;
			F2_2.Define( 0, 6 );
			F2_2.Define( 1, 8 );
			F2_2.Define( 2, 5 );
			F2_2.Define( 3, 22 );
			F2_2.Define( 4, 7 );
			F2_2.Define( 5, 2 );
			F2_2.Define( 6, 0 );
			F2_2.Define( 7, 4 );
			F2_2.Define( 8, 1 );
			F2_2.Define( 22, 3 );
			F2_2.Define( 54, 56 );
			F2_2.Define( 55, 58 );
			F2_2.Define( 56, 54 );
			F2_2.Define( 58, 55 );
			generatorSet.insert( F2_2 );

			Permutation F3_2;
			F3_2.Define( 0, 5 );
			F3_2.Define( 1, 2 );
			F3_2.Define( 2, 1 );
			F3_2.Define( 3, 7 );
			F3_2.Define( 4, 6 );
			F3_2.Define( 5, 0 );
			F3_2.Define( 6, 4 );
			F3_2.Define( 7, 3 );
			F3_2.Define( 8, 22 );
			F3_2.Define( 22, 8 );
			F3_2.Define( 55, 56 );
			F3_2.Define( 56, 55 );
			F3_2.Define( 57, 58 );
			F3_2.Define( 58, 57 );
			generatorSet.insert( F3_2 );

			Permutation F4_2;
			F4_2.Define( 0, 1 );
			F4_2.Define( 1, 0 );
			F4_2.Define( 2, 22 );
			F4_2.Define( 3, 6 );
			F4_2.Define( 4, 5 );
			F4_2.Define( 5, 4 );
			F4_2.Define( 6, 3 );
			F4_2.Define( 7, 8 );
			F4_2.Define( 8, 7 );
			F4_2.Define( 22, 2 );
			F4_2.Define( 54, 55 );
			F4_2.Define( 55, 54 );
			F4_2.Define( 56, 57 );
			F4_2.Define( 57, 56 );
			generatorSet.insert( F4_2 );

			// The generator set and domain was computed so that this would be the best base.
			// Or at least, that's the idea.
			for( uint i = 0; i < 59; i++ )
				baseArray.push_back(i);

			fileName = "SymGrpMadPuzzle7.txt";

			break;
		}
		case MixupCube:
		{
			Permutation permutation;

			// The numbering used here is the same as what might be used for the 4x4x4 Rubik's Cube,
			// but I don't believe the group will be nearly as big as that.

			// +Y face
			permutation.DefineIdentity();
			permutation.DefineCycle( 12, 13, 14, 15 );
			permutation.DefineCycle( 0, 3, 6, 9 );
			permutation.DefineCycle( 1, 4, 7, 10 );
			permutation.DefineCycle( 2, 5, 8, 11 );
			permutation.DefineCycle( 41, 16, 67, 54 );
			permutation.DefineCycle( 40, 27, 66, 53 );
			permutation.DefineCycle( 39, 26, 65, 52 );
			permutation.DefineCycle( 38, 25, 64, 51 );
			generatorSet.insert( permutation );

			// -Y face
			permutation.DefineIdentity();
			permutation.DefineCycle( 92, 93, 94, 95 );
			permutation.DefineCycle( 80, 83, 86, 89 );
			permutation.DefineCycle( 81, 84, 87, 90 );
			permutation.DefineCycle( 82, 85, 88, 91 );
			permutation.DefineCycle( 35, 48, 73, 22 );
			permutation.DefineCycle( 34, 59, 72, 21 );
			permutation.DefineCycle( 33, 58, 71, 20 );
			permutation.DefineCycle( 32, 57, 70, 19 );
			generatorSet.insert( permutation );

			// +X face
			permutation.DefineIdentity();
			permutation.DefineCycle( 28, 29, 30, 31 );
			permutation.DefineCycle( 16, 19, 22, 25 );
			permutation.DefineCycle( 17, 20, 23, 26 );
			permutation.DefineCycle( 18, 21, 24, 27 );
			permutation.DefineCycle( 38, 80, 70, 6 );
			permutation.DefineCycle( 37, 91, 69, 5 );
			permutation.DefineCycle( 36, 90, 68, 4 );
			permutation.DefineCycle( 35, 89, 67, 3 );
			generatorSet.insert( permutation );

			// -X face
			permutation.DefineIdentity();
			permutation.DefineCycle( 60, 61, 62, 63 );
			permutation.DefineCycle( 48, 51, 54, 57 );
			permutation.DefineCycle( 49, 52, 55, 58 );
			permutation.DefineCycle( 50, 53, 56, 59 );
			permutation.DefineCycle( 32, 0, 64, 86 );
			permutation.DefineCycle( 43, 11, 75, 85 );
			permutation.DefineCycle( 42, 10, 74, 84 );
			permutation.DefineCycle( 41, 9, 73, 83 );
			generatorSet.insert( permutation );

			// +Z face
			permutation.DefineIdentity();
			permutation.DefineCycle( 44, 45, 46, 47 );
			permutation.DefineCycle( 32, 35, 38, 41 );
			permutation.DefineCycle( 33, 36, 39, 42 );
			permutation.DefineCycle( 34, 37, 40, 43 );
			permutation.DefineCycle( 83, 19, 3, 51 );
			permutation.DefineCycle( 82, 18, 2, 50 );
			permutation.DefineCycle( 81, 17, 1, 49 );
			permutation.DefineCycle( 80, 16, 0, 48 );
			generatorSet.insert( permutation );

			// -Z face
			permutation.DefineIdentity();
			permutation.DefineCycle( 76, 77, 78, 79 );
			permutation.DefineCycle( 64, 67, 70, 73 );
			permutation.DefineCycle( 65, 68, 71, 74 );
			permutation.DefineCycle( 66, 69, 72, 75 );
			permutation.DefineCycle( 9, 25, 89, 57 );
			permutation.DefineCycle( 8, 24, 88, 56 );
			permutation.DefineCycle( 7, 23, 87, 55 );
			permutation.DefineCycle( 6, 22, 86, 54 );
			generatorSet.insert( permutation );

			// +X middle slice
			permutation.DefineIdentity();
			uint xSliceArrayA[] = { 14, 2, 46, 34, 92, 88, 78, 66 };
			permutation.DefineCycleArray( xSliceArrayA, 8 );
			uint xSliceArrayB[] = { 13, 39, 45, 81, 95, 71, 77, 7 };
			permutation.DefineCycleArray( xSliceArrayB, 8 );
			uint xSliceArrayC[] = { 15, 1, 47, 33, 93, 87, 79, 65 };
			permutation.DefineCycleArray( xSliceArrayC, 8 );
			uint xSliceArrayD[] = { 12, 40, 44, 82, 94, 72, 76, 8 };
			permutation.DefineCycleArray( xSliceArrayD, 8 );
			generatorSet.insert( permutation );

			// +Y middle slice
			permutation.DefineIdentity();
			uint ySliceArrayA[] = { 18, 30, 69, 79, 56, 60, 43, 45 };
			permutation.DefineCycleArray( ySliceArrayA, 8 );
			uint ySliceArrayB[] = { 36, 29, 23, 78, 74, 63, 49, 44 };
			permutation.DefineCycleArray( ySliceArrayB, 8 );
			uint ySliceArrayC[] = { 17, 31, 68, 76, 55, 61, 42, 46 };
			permutation.DefineCycleArray( ySliceArrayC, 8 );
			uint ySliceArrayD[] = { 37, 28, 24, 77, 75, 62, 50, 47 };
			permutation.DefineCycleArray( ySliceArrayD, 8 );
			generatorSet.insert( permutation );

			// +Z middle slice
			permutation.DefineIdentity();
			uint zSliceArrayA[] = { 29, 27, 13, 11, 61, 59, 93, 91 };
			permutation.DefineCycleArray( zSliceArrayA, 8 );
			uint zSliceArrayB[] = { 28, 4, 12, 52, 60, 84, 92, 20 };
			permutation.DefineCycleArray( zSliceArrayB, 8 );
			uint zSliceArrayC[] = { 30, 26, 14, 10, 62, 58, 94, 90 };
			permutation.DefineCycleArray( zSliceArrayC, 8 );
			uint zSliceArrayD[] = { 31, 5, 15, 53, 63, 85, 95, 21 };
			permutation.DefineCycleArray( zSliceArrayD, 8 );
			generatorSet.insert( permutation );

			// Stabilize 8 corners...
			baseArray.push_back(0);
			baseArray.push_back(3);
			baseArray.push_back(6);
			baseArray.push_back(9);
			baseArray.push_back(80);
			baseArray.push_back(83);
			baseArray.push_back(86);
			baseArray.push_back(89);

			// Stabilize 6 centers...
			baseArray.push_back(12);
			baseArray.push_back(92);
			baseArray.push_back(28);
			baseArray.push_back(60);
			baseArray.push_back(44);
			baseArray.push_back(76);

			// Stabilize 12 edges...
			baseArray.push_back(1);
			baseArray.push_back(4);
			baseArray.push_back(7);
			baseArray.push_back(10);
			baseArray.push_back(81);
			baseArray.push_back(84);
			baseArray.push_back(87);
			baseArray.push_back(90);
			baseArray.push_back(42);
			baseArray.push_back(17);
			baseArray.push_back(68);
			baseArray.push_back(55);

			fileName = "MixupCube.txt";

			// Note that we may be solving more than what is in the observable state of this puzzle.
			// Taking advantage of only what's observable has something to do with homomorphisms, I think.
			// In any case, I've never been able to get a firm intuitive grasp of such things.

			break;
		}
		case Bubbloid3x3x3:
		{
			Permutation permutation;

			permutation.DefineIdentity();
			permutation.Define( 0, 4 );
			permutation.Define( 4, 7 );
			permutation.Define( 7, 0 );
			permutation.Define( 12, 13 );
			permutation.Define( 13, 14 );
			permutation.Define( 14, 12 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 1, 5 );
			permutation.Define( 5, 4 );
			permutation.Define( 4, 1 );
			permutation.Define( 15, 16 );
			permutation.Define( 16, 17 );
			permutation.Define( 17, 15 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 2, 6 );
			permutation.Define( 6, 5 );
			permutation.Define( 5, 2 );
			permutation.Define( 18, 19 );
			permutation.Define( 19, 20 );
			permutation.Define( 20, 18 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 3, 7 );
			permutation.Define( 7, 6 );
			permutation.Define( 6, 3 );
			permutation.Define( 21, 22 );
			permutation.Define( 22, 23 );
			permutation.Define( 23, 21 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 0, 11 );
			permutation.Define( 11, 8 );
			permutation.Define( 8, 0 );
			permutation.Define( 24, 25 );
			permutation.Define( 25, 26 );
			permutation.Define( 26, 24 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 1, 8 );
			permutation.Define( 8, 9 );
			permutation.Define( 9, 1 );
			permutation.Define( 27, 28 );
			permutation.Define( 28, 29 );
			permutation.Define( 29, 27 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 2, 9 );
			permutation.Define( 9, 10 );
			permutation.Define( 10, 2 );
			permutation.Define( 30, 31 );
			permutation.Define( 31, 32 );
			permutation.Define( 32, 30 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 3, 10 );
			permutation.Define( 10, 11 );
			permutation.Define( 11, 3 );
			permutation.Define( 33, 34 );
			permutation.Define( 34, 35 );
			permutation.Define( 35, 33 );
			generatorSet.insert( permutation );

			baseArray.push_back( 12 );
			baseArray.push_back( 15 );
			baseArray.push_back( 18 );
			baseArray.push_back( 21 );
			baseArray.push_back( 24 );
			baseArray.push_back( 27 );
			baseArray.push_back( 30 );
			baseArray.push_back( 33 );
			baseArray.push_back( 0 );
			baseArray.push_back( 1 );
			baseArray.push_back( 2 );
			baseArray.push_back( 3 );
			baseArray.push_back( 4 );
			baseArray.push_back( 5 );
			baseArray.push_back( 6 );
			baseArray.push_back( 7 );
			baseArray.push_back( 8 );
			baseArray.push_back( 9 );
			baseArray.push_back( 10 );
			baseArray.push_back( 11 );

			fileName = "Bubbloid3x3x3.txt";

			break;
		}
		case Rubiks2x2x2:
		{
			Permutation permutation;

			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1, 2, 3 );
			permutation.DefineCycle( 13, 18, 7, 20 );
			permutation.DefineCycle( 14, 19, 4, 21 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 4, 5, 6, 7 );
			permutation.DefineCycle( 2, 18, 8, 22 );
			permutation.DefineCycle( 1, 17, 11, 21 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 9, 10, 11 );
			permutation.DefineCycle( 5, 16, 15, 22 );
			permutation.DefineCycle( 6, 17, 12, 23 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 12, 13, 14, 15 );
			permutation.DefineCycle( 0, 20, 10, 16 );
			permutation.DefineCycle( 3, 23, 9, 19 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 16, 17, 18, 19 );
			permutation.DefineCycle( 0, 12, 8, 4 );
			permutation.DefineCycle( 1, 13, 9, 5 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 20, 21, 22, 23 );
			permutation.DefineCycle( 2, 6, 10, 14 );
			permutation.DefineCycle( 3, 7, 11, 15 );
			generatorSet.insert( permutation );

			for( uint i = 0; i < 24; i++ )
				baseArray.push_back(i);

			fileName = "Rubiks2x2x2.txt";

			break;
		}
		case Rubiks3x3x3:
		{
			Permutation permutation;

			// +Y
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 5, 7, 2 );
			permutation.DefineCycle( 1, 3, 6, 4 );
			permutation.DefineCycle( 8, 16, 24, 32 );
			permutation.DefineCycle( 9, 17, 25, 33 );
			permutation.DefineCycle( 10, 18, 26, 34 );
			generatorSet.insert( permutation );

			// -X
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 13, 15, 10 );
			permutation.DefineCycle( 9, 11, 14, 12 );
			permutation.DefineCycle( 0, 39, 40, 16 );
			permutation.DefineCycle( 3, 36, 43, 19 );
			permutation.DefineCycle( 5, 34, 45, 21 );
			generatorSet.insert( permutation );

			// +Z
			permutation.DefineIdentity();
			permutation.DefineCycle( 16, 21, 23, 18 );
			permutation.DefineCycle( 17, 19, 22, 20 );
			permutation.DefineCycle( 5, 15, 42, 24 );
			permutation.DefineCycle( 6, 12, 41, 27 );
			permutation.DefineCycle( 7, 10, 40, 29 );
			generatorSet.insert( permutation );

			// +X
			permutation.DefineIdentity();
			permutation.DefineCycle( 24, 29, 31, 26 );
			permutation.DefineCycle( 25, 27, 30, 28 );
			permutation.DefineCycle( 2, 18, 42, 37 );
			permutation.DefineCycle( 4, 20, 44, 35 );
			permutation.DefineCycle( 7, 23, 47, 32 );
			generatorSet.insert( permutation );

			// -Z
			permutation.DefineIdentity();
			permutation.DefineCycle( 32, 37, 39, 34 );
			permutation.DefineCycle( 33, 35, 38, 36 );
			permutation.DefineCycle( 2, 31, 45, 8 );
			permutation.DefineCycle( 1, 28, 46, 11 );
			permutation.DefineCycle( 0, 26, 47, 13 );
			generatorSet.insert( permutation );

			// -Y
			permutation.DefineIdentity();
			permutation.DefineCycle( 40, 45, 47, 42 );
			permutation.DefineCycle( 41, 43, 46, 44 );
			permutation.DefineCycle( 13, 37, 29, 21 );
			permutation.DefineCycle( 14, 38, 30, 22 );
			permutation.DefineCycle( 15, 39, 31, 23 );
			generatorSet.insert( permutation );

			// The following would not normally be needed except
			// that my program lets the user move the middle slices,
			// and it's hard to pretend that doing so is a pair of
			// face turns.  So what we have here isn't the official
			// Rubik's Cube group, but it is necessary for my program
			// to work properly.  Essentially what it means is that
			// I consider different orientations of the puzzles as
			// different states as well.

			// X slice
			permutation.DefineIdentity();
			permutation.DefineCycle( 17, 41, 38, 1 );
			permutation.DefineCycle( 22, 46, 33, 6 );
			permutation.DefineCycle( 50, 48, 52, 53 );
			generatorSet.insert( permutation );

			// Y slice
			permutation.DefineIdentity();
			permutation.DefineCycle( 20, 28, 36, 12 );
			permutation.DefineCycle( 19, 27, 35, 11 );
			permutation.DefineCycle( 50, 51, 52, 49 );
			generatorSet.insert( permutation );

			// Z slice
			permutation.DefineIdentity();
			permutation.DefineCycle( 44, 25, 3, 14 );
			permutation.DefineCycle( 43, 30, 4, 9 );
			permutation.DefineCycle( 48, 51, 53, 49 );
			generatorSet.insert( permutation );

			// Stabilize 2 of the 3 centers to get all centers stabilized.
			baseArray.push_back( 51 );
			baseArray.push_back( 53 );

			// Stabilize corners...
			baseArray.push_back( 5 );
			baseArray.push_back( 7 );
			baseArray.push_back( 15 );
			baseArray.push_back( 23 );
			baseArray.push_back( 0 );
			baseArray.push_back( 2 );
			baseArray.push_back( 45 );
			baseArray.push_back( 47 );

			// Stabilize edges...
			baseArray.push_back( 12 );
			baseArray.push_back( 20 );
			baseArray.push_back( 17 );
			baseArray.push_back( 22 );
			baseArray.push_back( 3 );
			baseArray.push_back( 4 );
			baseArray.push_back( 44 );
			baseArray.push_back( 43 );
			baseArray.push_back( 35 );
			baseArray.push_back( 36 );
			baseArray.push_back( 33 );
			baseArray.push_back( 38 );

			fileName = "Rubiks3x3x3.txt";

			break;
		}
		case Rubiks2x3x3:
		{
			Permutation permutation;

			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 2, 4, 6 );
			permutation.DefineCycle( 1, 3, 5, 7 );
			permutation.DefineCycle( 8, 11, 14, 17 );
			permutation.DefineCycle( 9, 12, 15, 18 );
			permutation.DefineCycle( 10, 13, 16, 19 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 34 );
			permutation.DefineCycle( 1, 33 );
			permutation.DefineCycle( 2, 32 );
			permutation.DefineCycle( 11, 31 );
			permutation.DefineCycle( 19, 23 );
			permutation.DefineCycle( 8, 22 );
			permutation.DefineCycle( 9, 21 );
			permutation.DefineCycle( 10, 20 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 2, 36 );
			permutation.DefineCycle( 3, 35 );
			permutation.DefineCycle( 4, 34 );
			permutation.DefineCycle( 10, 26 );
			permutation.DefineCycle( 14, 22 );
			permutation.DefineCycle( 11, 25 );
			permutation.DefineCycle( 12, 24 );
			permutation.DefineCycle( 13, 23 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 4, 38 );
			permutation.DefineCycle( 5, 37 );
			permutation.DefineCycle( 6, 36 );
			permutation.DefineCycle( 13, 29 );
			permutation.DefineCycle( 17, 25 );
			permutation.DefineCycle( 14, 28 );
			permutation.DefineCycle( 15, 27 );
			permutation.DefineCycle( 16, 26 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 6, 32 );
			permutation.DefineCycle( 7, 39 );
			permutation.DefineCycle( 0, 38 );
			permutation.DefineCycle( 8, 28 );
			permutation.DefineCycle( 16, 20 );
			permutation.DefineCycle( 17, 31 );
			permutation.DefineCycle( 18, 30 );
			permutation.DefineCycle( 19, 29 );
			generatorSet.insert( permutation );

			baseArray.push_back( 0 );
			baseArray.push_back( 2 );
			baseArray.push_back( 4 );
			baseArray.push_back( 6 );
			baseArray.push_back( 32 );
			baseArray.push_back( 34 );
			baseArray.push_back( 36 );
			baseArray.push_back( 38 );
			baseArray.push_back( 1 );
			baseArray.push_back( 3 );
			baseArray.push_back( 5 );
			baseArray.push_back( 7 );
			baseArray.push_back( 33 );
			baseArray.push_back( 35 );
			baseArray.push_back( 37 );
			baseArray.push_back( 39 );

			fileName = "Rubiks2x3x3.txt";

			break;
		}
		case Rubiks2x2x3:
		{
			Permutation permutation;

			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1, 2, 3 );
			permutation.DefineCycle( 4, 11, 19, 26 );
			permutation.DefineCycle( 9, 10, 18, 25 );
			generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 28, 29, 30, 31 );
			permutation.DefineCycle( 6, 22, 21, 13 );
			permutation.DefineCycle( 7, 23, 16, 14 );
			generatorSet.insert( permutation );

			// +X
			permutation.DefineIdentity();
			permutation.DefineCycle( 1, 31 );
			permutation.DefineCycle( 2, 28 );
			permutation.DefineCycle( 4, 7 );
			permutation.DefineCycle( 6, 9 );
			permutation.DefineCycle( 5, 8 );
			permutation.DefineCycle( 11, 23 );
			permutation.DefineCycle( 12, 24 );
			permutation.DefineCycle( 13, 25 );
			generatorSet.insert( permutation );

			// +Z
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 28 );
			permutation.DefineCycle( 1, 29 );
			permutation.DefineCycle( 26, 23 );
			permutation.DefineCycle( 27, 24 );
			permutation.DefineCycle( 22, 25 );
			permutation.DefineCycle( 6, 18 );
			permutation.DefineCycle( 5, 17 );
			permutation.DefineCycle( 4, 16 );
			generatorSet.insert( permutation );

			// -X
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 30 );
			permutation.DefineCycle( 3, 29 );
			permutation.DefineCycle( 10, 22 );
			permutation.DefineCycle( 15, 27 );
			permutation.DefineCycle( 14, 26 );
			permutation.DefineCycle( 21, 18 );
			permutation.DefineCycle( 20, 17 );
			permutation.DefineCycle( 19, 16 );
			generatorSet.insert( permutation );

			// -Z
			permutation.DefineIdentity();
			permutation.DefineCycle( 2, 30 );
			permutation.DefineCycle( 3, 31 );
			permutation.DefineCycle( 9, 21 );
			permutation.DefineCycle( 8, 20 );
			permutation.DefineCycle( 7, 19 );
			permutation.DefineCycle( 11, 14 );
			permutation.DefineCycle( 15, 12 );
			permutation.DefineCycle( 10, 13 );
			generatorSet.insert( permutation );

			baseArray.push_back( 5 );
			baseArray.push_back( 12 );
			baseArray.push_back( 20 );
			baseArray.push_back( 27 );

			baseArray.push_back( 25 );
			baseArray.push_back( 9 );
			baseArray.push_back( 10 );
			baseArray.push_back( 18 );

			baseArray.push_back( 23 );
			baseArray.push_back( 7 );
			baseArray.push_back( 14 );
			baseArray.push_back( 16 );

			fileName = "Rubiks2x2x3.txt";

			break;
		}
		case Rubiks3x3x3_LL:
		{
			Permutation permutation;

			// Quarter turn of the top.
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 7, 19, 12 );
			permutation.DefineCycle( 1, 11, 18, 8 );
			permutation.DefineCycle( 2, 16, 17, 3 );
			permutation.DefineCycle( 4, 6, 15, 13 );
			permutation.DefineCycle( 5, 10, 14, 9 );
			generatorSet.insert( permutation );

#if 0
			// Flip adjacent edges.
			permutation.DefineIdentity();
			permutation.DefineCycle( 1, 5 );
			permutation.DefineCycle( 10, 11 );
			generatorSet.insert( permutation );

			// Twist adjacent corners.
			permutation.DefineIdentity();
			permutation.DefineCycle( 2, 6, 7 );
			permutation.DefineCycle( 15, 16, 19 );
			generatorSet.insert( permutation );

			// Try-cycle edges.
			permutation.DefineIdentity();
			permutation.DefineCycle( 11, 1, 8 );
			permutation.DefineCycle( 10, 5, 9 );
			generatorSet.insert( permutation );

			// Tri-cycle corners.
			permutation.DefineIdentity();
			permutation.DefineCycle( 7, 0, 12 );
			permutation.DefineCycle( 2, 3, 17 );
			permutation.DefineCycle( 6, 4, 13 );
			generatorSet.insert( permutation );
#else
			// R, U, Ri, U, R, U2, Ri
			permutation.DefineIdentity();
			permutation.DefineCycle( 10, 5, 9 );
			permutation.DefineCycle( 11, 1, 8 );
			permutation.DefineCycle( 4, 16, 0, 15, 3, 19 );
			permutation.DefineCycle( 13, 6, 17, 2, 12, 7 );
			generatorSet.insert( permutation );

			// U, R, Ui, Li, U, Ri, Ui, L
			permutation.DefineIdentity();
			permutation.DefineCycle( 7, 4, 17 );
			permutation.DefineCycle( 6, 3, 12 );
			permutation.DefineCycle( 2, 0, 13 );
			generatorSet.insert( permutation );

			// F, R, U, Ri, Ui, Fi
			permutation.DefineIdentity();
			permutation.DefineCycle( 14, 11, 1 );
			permutation.DefineCycle( 18, 10, 5 );
			permutation.DefineCycle( 13, 16, 17, 19, 12, 15 );
			permutation.DefineCycle( 4, 7, 0, 2, 3, 6 );
			generatorSet.insert( permutation );
#endif

			// Position and orient corners.
			baseArray.push_back(4);
			baseArray.push_back(15);
			baseArray.push_back(6);
			baseArray.push_back(13);

			// Position and orient edges.
			baseArray.push_back(5);
			baseArray.push_back(14);
			baseArray.push_back(9);
			baseArray.push_back(10);

			fileName = "Rubiks3x3x3_LL.txt";

			break;
		}
	}

	return fileName;
}

// Test.cpp