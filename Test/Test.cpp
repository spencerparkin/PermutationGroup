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
	MixupCube,
	SymGroup,
};

int main( int argc, char** argv )
{
	clock_t t = clock();

	StabilizerChain* stabChain = new StabilizerChain();
	Permutation permutation;
	UintArray baseArray;
	const char* fileName = nullptr;
	Puzzle puzzle = MixupCube;
	PermutationSet generatorSet;

	stabChain->logStream = &std::cout;

	switch( puzzle )
	{
		case SymGroup:
		{
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
		case MixupCube:
		{
			// TODO: I think this is actually wrong, and would not model the MixupCube as a permutation group.
			//       The problem is vertices shared between faces that slip past one another.  Try again, but
			//       number the faces, not the vertices.  To do this, subdivide the faces so that they become
			//       consistent.  We can imagine the cube being deformed as its moved, even if this isn't the
			//       physical reality of the puzzle.

			// +Y face
			permutation.DefineIdentity();
			permutation.DefineCycle( 0, 1, 2, 3 );
			permutation.DefineCycle( 26, 28, 31, 25 );
			permutation.DefineCycle( 27, 29, 30, 24 );
			permutation.DefineCycle( 19, 4, 21, 14 );
			permutation.DefineCycle( 18, 7, 20, 13 );
			permutation.DefineCycle( 39, 36, 34, 33 );
			generatorSet.insert( permutation );

			// -Y face
			permutation.DefineIdentity();
			permutation.DefineCycle( 8, 9, 10, 11 );
			permutation.DefineCycle( 45, 42, 46, 41 );
			permutation.DefineCycle( 44, 43, 47, 40 );
			permutation.DefineCycle( 17, 12, 23, 6 );
			permutation.DefineCycle( 16, 15, 22, 5 );
			permutation.DefineCycle( 32, 35, 37, 38 );
			generatorSet.insert( permutation );

			// +X face
			permutation.DefineIdentity();
			permutation.DefineCycle( 12, 13, 14, 15 );
			permutation.DefineCycle( 32, 24, 34, 43 );
			permutation.DefineCycle( 33, 25, 35, 42 );
			permutation.DefineCycle( 16, 0, 20, 10 );
			permutation.DefineCycle( 19, 3, 23, 9 );
			permutation.DefineCycle( 26, 30, 46, 44 );
			generatorSet.insert( permutation );

			// -X face
			permutation.DefineIdentity();
			permutation.DefineCycle( 4, 5, 6, 7 );
			permutation.DefineCycle( 39, 40, 37, 29 );
			permutation.DefineCycle( 38, 41, 36, 28 );
			permutation.DefineCycle( 18, 8, 22, 2 );
			permutation.DefineCycle( 17, 11, 12, 1 );
			permutation.DefineCycle( 45, 47, 31, 27 );
			generatorSet.insert( permutation );

			// +Z face
			permutation.DefineIdentity();
			permutation.DefineCycle( 20, 21, 22, 23 );
			permutation.DefineCycle( 30, 36, 47, 35 );
			permutation.DefineCycle( 31, 37, 46, 34 );
			permutation.DefineCycle( 3, 7, 11, 15 );
			permutation.DefineCycle( 2, 6, 10, 14 );
			permutation.DefineCycle( 29, 41, 43, 25 );
			generatorSet.insert( permutation );

			// -Z face
			permutation.DefineIdentity();
			permutation.DefineCycle( 16, 17, 18, 19 );
			permutation.DefineCycle( 44, 38, 27, 33 );
			permutation.DefineCycle( 45, 39, 26, 32 );
			permutation.DefineCycle( 9, 5, 1, 13 );
			permutation.DefineCycle( 8, 4, 0, 12 );
			permutation.DefineCycle( 40, 28, 24, 42 );
			generatorSet.insert( permutation );

			// +X middle slice
			permutation.DefineIdentity();
			uint xSliceArrayA[] = { 0, 3, 30, 20, 23, 46, 10, 9, 44, 16, 19, 26 };
			permutation.DefineCycleArray( xSliceArrayA, 12 );
			uint xSliceArrayB[] = { 1, 2, 31, 21, 22, 47, 11, 8, 45, 17, 18, 27 };
			permutation.DefineCycleArray( xSliceArrayB, 12 );
			generatorSet.insert( permutation );

			// +Y middle slice
			permutation.DefineIdentity();
			uint ySliceArrayA[] = { 21, 20, 34, 14, 13, 33, 19, 18, 39, 4, 7, 36 };
			permutation.DefineCycleArray( ySliceArrayA, 12 );
			uint ySliceArrayB[] = { 22, 23, 35, 15, 12, 32, 16, 17, 38, 5, 6, 37 };
			permutation.DefineCycleArray( ySliceArrayB, 12 );
			generatorSet.insert( permutation );

			// +Z middle slice
			permutation.DefineIdentity();
			uint zSliceArrayA[] = { 3, 2, 29, 7, 6, 41, 11, 10, 43, 15, 14, 25 };
			permutation.DefineCycleArray( zSliceArrayA, 12 );
			uint zSliceArrayB[] = { 0, 1, 28, 4, 5, 40, 8, 9, 42, 12, 13, 24 };
			permutation.DefineCycleArray( zSliceArrayB, 12 );
			generatorSet.insert( permutation );

			for( uint i = 0; i < 48; i++ )
				baseArray.push_back(i);

			fileName = "MixupCube.txt";

			break;
		}
		case Bubbloid3x3x3:
		{
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
	}

	bool success = stabChain->Generate( generatorSet, baseArray );
	if( !success )
		std::cout << "Failed!\n";

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	if( success )
	{
		unsigned long long order = stabChain->group->Order();

		stabChain->Print( std::cout );

		stabChain->group->NameGenerators();

		CompressInfo compressInfo;
		stabChain->group->MakeCompressInfo( compressInfo );

		class TestPermutationStreamCreator : public PermutationStreamCreator
		{
		public:
			virtual PermutationStream* CreateForGroup( StabilizerChain::Group* group, const CompressInfo* compressInfo ) override
			{
				return new PermutationFreeGroupStream( &group->generatorSet, compressInfo );
			}
		};

		TestPermutationStreamCreator streamCreator;

		if( stabChain->OptimizeNames( streamCreator, compressInfo, 20.0 ) )
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

// Test.cpp