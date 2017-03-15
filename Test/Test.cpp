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
	permutation.Define( 4, 5 );
	permutation.Define( 5, 4 );
	permutation.Define( 8, 9 );
	permutation.Define( 9, 8 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "b" );
	permutation.Define( 3, 5 );
	permutation.Define( 5, 3 );
	permutation.Define( 8, 9 );
	permutation.Define( 9, 8 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "c" );
	permutation.Define( 3, 4 );
	permutation.Define( 4, 3 );
	permutation.Define( 1, 8 );
	permutation.Define( 8, 1 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "d" );
	permutation.Define( 0, 1 );
	permutation.Define( 1, 0 );
	permutation.Define( 3, 7 );
	permutation.Define( 7, 3 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "e" );
	permutation.Define( 1, 2 );
	permutation.Define( 2, 1 );
	permutation.Define( 3, 6 );
	permutation.Define( 6, 3 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.SetName( "f" );
	permutation.Define( 0, 2 );
	permutation.Define( 2, 0 );
	permutation.Define( 6, 7 );
	permutation.Define( 7, 6 );
	chainGroup->generatorSet.insert( permutation );

	chainGroup->Generate( &std::cout );

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	//chainGroup->Print( std::cout );

	getchar();

	// TODO: Actually use the stabilizer chain here to factor a permutation.
}

int main( int argc, char** argv )
{
	StabilizerChainTest();

	return 0;
}

// Test.cpp