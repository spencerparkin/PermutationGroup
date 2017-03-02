// Test.cpp

#include <iostream>
#include <time.h>
#include "ElementSet.h"
#include "StabilizerChain.h"

int main( int argc, char** argv )
{
	clock_t t = clock();

	std::cout << "Generating stabilizer chain...\n";

	StabilizerChainGroup* chainGroup = new StabilizerChainGroup();

	PermutationElement* element = nullptr;

	element = new PermutationElement();
	element->word.SetName( "a" );
	element->permutation.Define( 0, 4 );
	element->permutation.Define( 4, 7 );
	element->permutation.Define( 7, 0 );
	chainGroup->generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "b" );
	element->permutation.Define( 1, 5 );
	element->permutation.Define( 5, 4 );
	element->permutation.Define( 4, 1 );
	chainGroup->generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "c" );
	element->permutation.Define( 2, 6 );
	element->permutation.Define( 6, 5 );
	element->permutation.Define( 5, 2 );
	chainGroup->generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "d" );
	element->permutation.Define( 3, 7 );
	element->permutation.Define( 7, 6 );
	element->permutation.Define( 6, 3 );
	chainGroup->generatorSet.AddNewMember( element );

	NaturalNumberSet domainSet;
	for( uint i = 0; i < 8; i++ )
		domainSet.AddMember(i);

	chainGroup->Generate( domainSet, &std::cout );

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	getchar();

	return 0;
}

// Test.cpp