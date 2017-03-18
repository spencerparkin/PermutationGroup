// Test.cpp

#include <iostream>
#include <fstream>
#include <time.h>
#include "StabilizerChain.h"

void Bubbloid3x3x3( void )
{
	clock_t t = clock();

	std::cout << "Generating stabilizer chain...\n";

	StabilizerChainGroup* chainGroup = new StabilizerChainGroup();

	Permutation permutation;

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

	uint pointArray[20] = { 12, 15, 18, 21, 24, 27, 30, 33, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	uint pointArraySize = sizeof( pointArray ) / sizeof( uint );

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
	fstream.open( "Bubbloid3x3x3.txt", std::fstream::out );
	fstream << jsonString;
	fstream.close();

	getchar();

	delete chainGroup;
}

int main( int argc, char** argv )
{
	Bubbloid3x3x3();

	return 0;
}

// Test.cpp