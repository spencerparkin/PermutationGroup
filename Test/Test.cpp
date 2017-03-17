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
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.Define( 1, 5 );
	permutation.Define( 5, 4 );
	permutation.Define( 4, 1 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.Define( 2, 6 );
	permutation.Define( 6, 5 );
	permutation.Define( 5, 2 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.Define( 3, 7 );
	permutation.Define( 7, 6 );
	permutation.Define( 6, 3 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.Define( 0, 11 );
	permutation.Define( 11, 8 );
	permutation.Define( 8, 0 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.Define( 1, 8 );
	permutation.Define( 8, 9 );
	permutation.Define( 9, 1 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.Define( 2, 9 );
	permutation.Define( 9, 10 );
	permutation.Define( 10, 2 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.Define( 3, 10 );
	permutation.Define( 10, 11 );
	permutation.Define( 11, 3 );
	chainGroup->generatorSet.insert( permutation );

	uint pointArray[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	uint pointArraySize = sizeof( pointArray ) / sizeof( uint );

	chainGroup->Generate( pointArray, pointArraySize, 0, false, &std::cout );

	std::cout << "Optimizing stabilizer chain...\n";

	chainGroup->NameGenerators();
	chainGroup->Optimize( &std::cout );

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

	// TODO: Actually use the stabilizer chain here to factor a permutation.
}

int main( int argc, char** argv )
{
	Bubbloid3x3x3();

	return 0;
}

// Test.cpp