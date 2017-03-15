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
	permutation.Define( 0, 1 );
	permutation.Define( 1, 2 );
	permutation.Define( 2, 0 );
	chainGroup->generatorSet.insert( permutation );

	permutation.DefineIdentity();
	permutation.Define( 1, 3 );
	permutation.Define( 3, 4 );
	permutation.Define( 4, 1 );
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