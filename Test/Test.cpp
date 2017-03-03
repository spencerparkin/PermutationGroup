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
	element->permutation.Define( 0, 1 );
	element->permutation.Define( 1, 2 );
	element->permutation.Define( 2, 0 );
	chainGroup->generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "b" );
	element->permutation.Define( 2, 3 );
	element->permutation.Define( 3, 4 );
	element->permutation.Define( 4, 2 );
	chainGroup->generatorSet.AddNewMember( element );

	ConfiguredWordCompressor wordCompressor;
	wordCompressor.Configure( chainGroup->generatorSet );

	NaturalNumberSet domainSet;
	for( uint i = 0; i < 5; i++ )
		domainSet.AddMember(i);

	chainGroup->Generate( domainSet, &wordCompressor );

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	chainGroup->Print( std::cout );

	getchar();

	return 0;
}

// Test.cpp