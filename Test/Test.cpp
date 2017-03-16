// Test.cpp

#include <iostream>
#include <time.h>
#include "StabilizerChain.h"

void StabilizerChainTest( void )
{
	clock_t t = clock();

	std::cout << "Generating stabilizer chain...\n";

	StabilizerChainGroup* chainGroup = new StabilizerChainGroup();

	Permutation permutation;

	permutation.DefineIdentity();
	permutation.SetName( "a" );
	permutation.Define( 0, 4 );
	permutation.Define( 4, 7 );
	permutation.Define( 7, 0 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "b" );
	permutation.Define( 1, 5 );
	permutation.Define( 5, 4 );
	permutation.Define( 4, 1 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "c" );
	permutation.Define( 2, 6 );
	permutation.Define( 6, 5 );
	permutation.Define( 5, 2 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "d" );
	permutation.Define( 3, 7 );
	permutation.Define( 7, 6 );
	permutation.Define( 6, 3 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "e" );
	permutation.Define( 0, 11 );
	permutation.Define( 11, 8 );
	permutation.Define( 8, 0 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "f" );
	permutation.Define( 1, 8 );
	permutation.Define( 8, 9 );
	permutation.Define( 9, 1 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "g" );
	permutation.Define( 2, 9 );
	permutation.Define( 9, 10 );
	permutation.Define( 10, 2 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "h" );
	permutation.Define( 3, 10 );
	permutation.Define( 10, 11 );
	permutation.Define( 11, 3 );
	chainGroup->generatorSet.insert( permutation );

	uint pointArray[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	uint pointArraySize = sizeof( pointArray ) / sizeof( uint );

	chainGroup->Generate( pointArray, pointArraySize, 0, &std::cout );

	std::cout << "Optimizing stabilizer chain...\n";

	chainGroup->Optimize( &std::cout );

	chainGroup->Print( std::cout );

	std::string jsonString;
	chainGroup->SaveToJsonString( jsonString );

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	getchar();

	// TODO: Actually use the stabilizer chain here to factor a permutation.
}

int main( int argc, char** argv )
{
	StabilizerChainTest();

	return 0;
}

// Test.cpp