// Test.cpp

#include <iostream>
#include <time.h>
#include "ElementSet.h"
#include "StabilizerChain.h"

void StabilizerChainTest( void )
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

	ConfiguredWordCompressor wordCompressor;
	wordCompressor.Configure( chainGroup->generatorSet );

	NaturalNumberSet domainSet;
	for( uint i = 0; i < 8; i++ )
		domainSet.AddMember(i);

	chainGroup->Generate( domainSet, &wordCompressor, &std::cout );

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	chainGroup->Print( std::cout );

	getchar();
}

void GroupGenerationTest( void )
{
	// TODO: Also test this with a factor group.

	clock_t t = clock();

	PermutationSet generatorSet;

	PermutationElement* element = nullptr;

	element = new PermutationElement();
	element->word.SetName( "a" );
	element->permutation.Define( 0, 4 );
	element->permutation.Define( 4, 3 );
	element->permutation.Define( 3, 0 );
	generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "b" );
	element->permutation.Define( 4, 1 );
	element->permutation.Define( 1, 5 );
	element->permutation.Define( 5, 4 );
	generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "c" );
	element->permutation.Define( 3, 5 );
	element->permutation.Define( 5, 2 );
	element->permutation.Define( 2, 3 );
	generatorSet.AddNewMember( element );

	ConfiguredWordCompressor wordCompressor;
	wordCompressor.Configure( generatorSet );

	PermutationSet group;

	bool generated = false;
	
	if( false )
		generated = group.GenerateGroupResursive( generatorSet, &std::cout );
	else
		generated = group.GenerateGroup( generatorSet, &std::cout );

	if( !generated )
		std::cout << "Fail!\n";
	else
	{
		wordCompressor.Compress( group );
		group.Print( std::cout );
	}

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	getchar();
}

int main( int argc, char** argv )
{
	//GroupGenerationTest();

	StabilizerChainTest();

	return 0;
}

// Test.cpp