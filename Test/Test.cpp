// Test.cpp

#include <iostream>
#include <fstream>
#include <time.h>
#include "StabilizerChain_Deprecated.h"

enum Puzzle
{
	Bubbloid3x3x3,
	Rubiks2x2x2,
	Rubiks3x3x3,
	Rubiks2x3x3,
};

int main( int argc, char** argv )
{
	clock_t t = clock();

	std::cout << "Generating stabilizer chain...\n";

	StabilizerChainGroup_Deprecated* chainGroup = new StabilizerChainGroup_Deprecated();

	Permutation permutation;

	uint* pointArray = nullptr;
	uint pointArraySize = 0;

	const char* fileName = nullptr;

	Puzzle puzzle = Rubiks3x3x3;

	switch( puzzle )
	{
		case Bubbloid3x3x3:
		{
			permutation.DefineIdentity();
			permutation.Define( 0, 4 );
			permutation.Define( 4, 7 );
			permutation.Define( 7, 0 );
			permutation.Define( 12, 13 );
			permutation.Define( 13, 14 );
			permutation.Define( 14, 12 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 1, 5 );
			permutation.Define( 5, 4 );
			permutation.Define( 4, 1 );
			permutation.Define( 15, 16 );
			permutation.Define( 16, 17 );
			permutation.Define( 17, 15 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 2, 6 );
			permutation.Define( 6, 5 );
			permutation.Define( 5, 2 );
			permutation.Define( 18, 19 );
			permutation.Define( 19, 20 );
			permutation.Define( 20, 18 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 3, 7 );
			permutation.Define( 7, 6 );
			permutation.Define( 6, 3 );
			permutation.Define( 21, 22 );
			permutation.Define( 22, 23 );
			permutation.Define( 23, 21 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 0, 11 );
			permutation.Define( 11, 8 );
			permutation.Define( 8, 0 );
			permutation.Define( 24, 25 );
			permutation.Define( 25, 26 );
			permutation.Define( 26, 24 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 1, 8 );
			permutation.Define( 8, 9 );
			permutation.Define( 9, 1 );
			permutation.Define( 27, 28 );
			permutation.Define( 28, 29 );
			permutation.Define( 29, 27 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 2, 9 );
			permutation.Define( 9, 10 );
			permutation.Define( 10, 2 );
			permutation.Define( 30, 31 );
			permutation.Define( 31, 32 );
			permutation.Define( 32, 30 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.Define( 3, 10 );
			permutation.Define( 10, 11 );
			permutation.Define( 11, 3 );
			permutation.Define( 33, 34 );
			permutation.Define( 34, 35 );
			permutation.Define( 35, 33 );
			chainGroup->generatorSet.insert( permutation );

			static uint __pointArray[20] = { 12, 15, 18, 21, 24, 27, 30, 33, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
			uint __pointArraySize = sizeof( __pointArray ) / sizeof( uint );

			pointArray = __pointArray;
			pointArraySize = __pointArraySize;

			fileName = "Bubbloid3x3x3.txt";

			break;
		}
		case Rubiks2x2x2:
		{
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1, 2, 3 );
			permutation.DefineCycle( 13, 18, 7, 20 );
			permutation.DefineCycle( 14, 19, 4, 21 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 4, 5, 6, 7 );
			permutation.DefineCycle( 2, 18, 8, 22 );
			permutation.DefineCycle( 1, 17, 11, 21 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 9, 10, 11 );
			permutation.DefineCycle( 5, 16, 15, 22 );
			permutation.DefineCycle( 6, 17, 12, 23 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 12, 13, 14, 15 );
			permutation.DefineCycle( 0, 20, 10, 16 );
			permutation.DefineCycle( 3, 23, 9, 19 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 16, 17, 18, 19 );
			permutation.DefineCycle( 0, 12, 8, 4 );
			permutation.DefineCycle( 1, 13, 9, 5 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 20, 21, 22, 23 );
			permutation.DefineCycle( 2, 6, 10, 14 );
			permutation.DefineCycle( 3, 7, 11, 15 );
			chainGroup->generatorSet.insert( permutation );

			static uint __pointArray[24] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23 };
			uint __pointArraySize = sizeof( __pointArray ) / sizeof( uint );

			pointArray = __pointArray;
			pointArraySize = __pointArraySize;

			fileName = "Rubiks2x2x2.txt";

			break;
		}
		case Rubiks3x3x3:
		{
			// TODO: Unless I can reduce the number of Schreier generators, this
			//       will always run out of memory.  Hmmm...  What if I came up with
			//       my own generators for each stage based on what I know about the 3x3x3?

			// +Y
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 5, 7, 2 );
			permutation.DefineCycle( 1, 3, 6, 4 );
			permutation.DefineCycle( 8, 16, 24, 32 );
			permutation.DefineCycle( 9, 17, 25, 33 );
			permutation.DefineCycle( 10, 18, 26, 34 );
			chainGroup->generatorSet.insert( permutation );

			// -X
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 13, 15, 10 );
			permutation.DefineCycle( 9, 11, 14, 12 );
			permutation.DefineCycle( 0, 39, 40, 16 );
			permutation.DefineCycle( 3, 36, 43, 19 );
			permutation.DefineCycle( 5, 34, 45, 21 );
			chainGroup->generatorSet.insert( permutation );

			// +Z
			permutation.DefineIdentity();
			permutation.DefineCycle( 16, 21, 23, 18 );
			permutation.DefineCycle( 17, 19, 22, 20 );
			permutation.DefineCycle( 5, 15, 42, 24 );
			permutation.DefineCycle( 6, 12, 41, 27 );
			permutation.DefineCycle( 7, 10, 40, 29 );
			chainGroup->generatorSet.insert( permutation );

			// +X
			permutation.DefineIdentity();
			permutation.DefineCycle( 24, 29, 31, 26 );
			permutation.DefineCycle( 25, 27, 30, 28 );
			permutation.DefineCycle( 2, 18, 42, 37 );
			permutation.DefineCycle( 4, 20, 44, 35 );
			permutation.DefineCycle( 7, 23, 47, 32 );
			chainGroup->generatorSet.insert( permutation );

			// -Z
			permutation.DefineIdentity();
			permutation.DefineCycle( 32, 37, 39, 34 );
			permutation.DefineCycle( 33, 35, 38, 36 );
			permutation.DefineCycle( 2, 31, 45, 8 );
			permutation.DefineCycle( 1, 28, 46, 11 );
			permutation.DefineCycle( 0, 26, 47, 13 );
			chainGroup->generatorSet.insert( permutation );

			// -Y
			permutation.DefineIdentity();
			permutation.DefineCycle( 40, 45, 47, 42 );
			permutation.DefineCycle( 41, 43, 46, 44 );
			permutation.DefineCycle( 13, 37, 29, 21 );
			permutation.DefineCycle( 14, 38, 30, 22 );
			permutation.DefineCycle( 15, 39, 31, 23 );
			chainGroup->generatorSet.insert( permutation );

			static uint __pointArray[20];

			// Stabilize corners...
			__pointArray[0] = 5;
			__pointArray[1] = 7;
			__pointArray[2] = 15;
			__pointArray[3] = 23;
			__pointArray[4] = 0;
			__pointArray[5] = 2;
			__pointArray[6] = 45;
			__pointArray[7] = 47;

			// Stabilize edges...
			__pointArray[8] = 12;
			__pointArray[9] = 20;
			__pointArray[10] = 17;
			__pointArray[11] = 22;
			__pointArray[12] = 3;
			__pointArray[13] = 4;
			__pointArray[14] = 44;
			__pointArray[15] = 43;
			__pointArray[16] = 35;
			__pointArray[17] = 36;
			__pointArray[18] = 33;
			__pointArray[19] = 38;

			uint __pointArraySize = sizeof( __pointArray ) / sizeof( uint );

			pointArray = __pointArray;
			pointArraySize = __pointArraySize;

			fileName = "Rubiks3x3x3.txt";

			break;
		}
		case Rubiks2x3x3:
		{
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 2, 4, 6 );
			permutation.DefineCycle( 1, 3, 5, 7 );
			permutation.DefineCycle( 8, 11, 14, 17 );
			permutation.DefineCycle( 9, 12, 15, 18 );
			permutation.DefineCycle( 10, 13, 16, 19 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 34 );
			permutation.DefineCycle( 1, 33 );
			permutation.DefineCycle( 2, 32 );
			permutation.DefineCycle( 11, 31 );
			permutation.DefineCycle( 19, 23 );
			permutation.DefineCycle( 8, 22 );
			permutation.DefineCycle( 9, 21 );
			permutation.DefineCycle( 10, 20 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 2, 36 );
			permutation.DefineCycle( 3, 35 );
			permutation.DefineCycle( 4, 34 );
			permutation.DefineCycle( 10, 26 );
			permutation.DefineCycle( 14, 22 );
			permutation.DefineCycle( 11, 25 );
			permutation.DefineCycle( 12, 24 );
			permutation.DefineCycle( 13, 23 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 4, 38 );
			permutation.DefineCycle( 5, 37 );
			permutation.DefineCycle( 6, 36 );
			permutation.DefineCycle( 13, 29 );
			permutation.DefineCycle( 17, 25 );
			permutation.DefineCycle( 14, 28 );
			permutation.DefineCycle( 15, 27 );
			permutation.DefineCycle( 16, 26 );
			chainGroup->generatorSet.insert( permutation );

			permutation.DefineIdentity();
			permutation.DefineCycle( 6, 32 );
			permutation.DefineCycle( 7, 39 );
			permutation.DefineCycle( 0, 38 );
			permutation.DefineCycle( 8, 28 );
			permutation.DefineCycle( 16, 20 );
			permutation.DefineCycle( 17, 31 );
			permutation.DefineCycle( 18, 30 );
			permutation.DefineCycle( 19, 29 );
			chainGroup->generatorSet.insert( permutation );

			static uint __pointArray[16] =
			{
				0, 2, 4, 6, 32, 34, 36, 38,
				1, 3, 5, 7, 33, 35, 37, 39
			};

			uint __pointArraySize = sizeof( __pointArray ) / sizeof( uint );

			pointArray = __pointArray;
			pointArraySize = __pointArraySize;

			fileName = "Rubiks2x3x3.txt";

			break;
		}
	}

	chainGroup->Generate( pointArray, pointArraySize, 0, false, &std::cout );

	std::cout << "Optimizing stabilizer chain...\n";

	chainGroup->NameGenerators();

	CompressInfo compressInfo;
	chainGroup->MakeCompressInfo( compressInfo );

	chainGroup->Optimize( compressInfo, &std::cout );

	chainGroup->Print( std::cout );

	std::string jsonString;
	chainGroup->SaveToJsonString( jsonString );

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	std::fstream fstream;
	fstream.open( fileName, std::fstream::out );
	fstream << jsonString;
	fstream.close();

	getchar();

	delete chainGroup;

	return 0;
}

// Test.cpp