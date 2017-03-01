// Test.cpp

#include <iostream>
#include <time.h>
#include "ElementSet.h"

int main( int argc, char** argv )
{
	clock_t t = clock();

	ElementCollection group;

	Element element;

	element.word.SetName( "a" );
	element.permutation.Define( 0, 1 );
	element.permutation.Define( 1, 2 );
	element.permutation.Define( 2, 0 );
	group.AddElement( element );

	element.Identity();
	element.word.SetName( "b" );
	element.permutation.Define( 2, 3 );
	element.permutation.Define( 3, 0 );
	element.permutation.Define( 0, 2 );
	group.AddElement( element );

	element.Identity();
	element.word.SetName( "c" );
	element.permutation.Define( 0, 3 );
	element.permutation.Define( 3, 0 );
	group.AddElement( element );

	element.Identity();
	element.word.SetName( "d" );
	element.permutation.Define( 0, 4 );
	element.permutation.Define( 4, 0 );
	group.AddElement( element );

	group.GenerateGroup( &std::cout );

	group.Print( std::cout );

	t = clock() - t;
	double elapsed_time = double(t) / double( CLOCKS_PER_SEC );
	std::cout << "Time taken: " << elapsed_time << " sec\n";

	getchar();

	return 0;
}

// Test.cpp