// Test.cpp

#include <iostream>
#include <time.h>
#include "ElementSet.h"
#include "StabilizerChain.h"

#if 0
void StabilizerChainTest( void )
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
	element->permutation.Define( 1, 3 );
	element->permutation.Define( 3, 4 );
	element->permutation.Define( 4, 1 );
	chainGroup->generatorSet.AddNewMember( element );

	/*element = new PermutationElement();
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
	chainGroup->generatorSet.AddNewMember( element );*/

	ConfiguredWordCompressor wordCompressor;
	wordCompressor.Configure( chainGroup->generatorSet );

	NaturalNumberSet domainSet;
	for( uint i = 0; i < 5; i++ )
		domainSet.AddMember(i);

	chainGroup->Generate( domainSet, &wordCompressor, &std::cout );

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	chainGroup->Print( std::cout );

	getchar();

	// TODO: Actually use the stabilizer chain here to factor a permutation.
}
#endif

void GroupGenerationTest( void )
{
	// TODO: Also test this with a factor group.

	clock_t t = clock();

	PermutationSet generatorSet;
	generatorSet.elementsHaveUniqueRepresentation = true;

	PermutationElement* element = nullptr;

	element = new PermutationElement();
	element->word.SetName( "a" );
	element->permutation.Define( 0, 1 );
	element->permutation.Define( 1, 0 );
	generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "b" );
	element->permutation.Define( 1, 2 );
	element->permutation.Define( 2, 1 );
	generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "c" );
	element->permutation.Define( 2, 3 );
	element->permutation.Define( 3, 2 );
	generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "d" );
	element->permutation.Define( 3, 4 );
	element->permutation.Define( 4, 3 );
	generatorSet.AddNewMember( element );

	element = new PermutationElement();
	element->word.SetName( "e" );
	element->permutation.Define( 4, 5 );
	element->permutation.Define( 5, 4 );
	generatorSet.AddNewMember( element );

	/*element = new PermutationElement();
	element->word.SetName( "f" );
	element->permutation.Define( 5, 6 );
	element->permutation.Define( 6, 5 );
	generatorSet.AddNewMember( element );*/

	ConfiguredWordCompressor wordCompressor;
	wordCompressor.Configure( generatorSet );

	PermutationSet* group = new PermutationSet();
	group->elementsHaveUniqueRepresentation = true;

	bool generated = group->Generate( generatorSet, &std::cout, true );
	if( !generated )
		std::cout << "Fail!\n";
	else
	{
		wordCompressor.Compress( *group );
		group->Print( std::cout );
	}

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	getchar();
}

int main( int argc, char** argv )
{
	GroupGenerationTest();

	//StabilizerChainTest();

	return 0;
}

// Test.cpp