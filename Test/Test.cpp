// Test.cpp

#include <iostream>
#include "ElementSet.h"

int main( int argc, char** argv )
{
	ElementCollection group;

	Element element;
	
	// BUG: This should generate a subgroup of S_4 which has order 4!=24,
	//      yet I'm finding an unending number of elements during generation.

	element.word.SetName( "a" );
	element.permutation.Define( 0, 1 );
	element.permutation.Define( 1, 2 );
	element.permutation.Define( 2, 0 );
	group.AddElement( element );

	element.word.SetName( "b" );
	element.permutation.Define( 2, 3 );
	element.permutation.Define( 3, 0 );
	element.permutation.Define( 0, 2 );
	group.AddElement( element );

	group.GenerateGroup( &std::cout );

	group.Print( std::cout );

	getchar();

	return 0;
}

// Test.cpp