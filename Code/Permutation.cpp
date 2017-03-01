// Permutation.cpp

#include "Permutation.h"
#include "NaturalNumberSet.h"
#include <sstream>

//------------------------------------------------------------------------------------------
//                                        Permutation
//------------------------------------------------------------------------------------------

Permutation::Permutation( void )
{
	map = new Map();
}

Permutation::Permutation( const Permutation& permutation )
{
	map = new Map();
	SetCopy( permutation );
}

/*virtual*/ Permutation::~Permutation( void )
{
	delete map;
}

uint Permutation::Evaluate( uint input ) const
{
	uint output = input;

	Map::iterator iter = map->find( input );
	if( iter != map->end() )
		output = iter->second;

	return output;
}

void Permutation::Define( uint input, uint output )
{
	map->insert( std::pair< uint, uint >( input, output ) );
}

std::size_t Permutation::CalcHash( void ) const
{
	PruneInfo info;
	Prune( &info );

	std::stringstream stream;

	for( Map::const_iterator iter = map->cbegin(); iter != map->cend(); iter++ )
		stream << iter->first << iter->second;

	std::string str = stream.str();
	std::size_t hash = std::hash< std::string >{}( str );

	return hash;
}

bool Permutation::IsValid( void ) const
{
	Permutation inverse;
	return GetInverse( inverse );
}

bool Permutation::IsEven( void ) const
{
	PermutationList permutationList;
	Factor( permutationList );

	int total = 0;
	for( PermutationList::iterator iter = permutationList.begin(); iter != permutationList.end(); iter++ )
	{
		// An N-cycle can be written as N-1 transpositions.
		Permutation* permutation = *iter;
		total += ( int )permutation->map->size() - 1;
	}

	Delete( permutationList );

	return( total % 2 == 0 ? true : false );
}

bool Permutation::IsOdd( void ) const
{
	return !IsEven();
}

bool Permutation::IsIdentity( void ) const
{
	Prune();

	return( map->size() == 0 ? true : false );
}

bool Permutation::IsEqualTo( const Permutation& permutation ) const
{
	Permutation inverse;
	if( !permutation.GetInverse( inverse ) )
		return false;

	Permutation product;
	product.Multiply( *this, inverse );

	return product.IsIdentity();
}

bool Permutation::CommutesWith( const Permutation& permutation ) const
{
	Permutation productA, productB;

	productA.Multiply( *this, permutation );
	productB.Multiply( permutation, *this );

	return productA.IsEqualTo( productB );
}

uint Permutation::Order( void ) const
{
	PermutationList permutationList;
	Factor( permutationList );

	NaturalNumberSet set;

	for( PermutationList::iterator iter = permutationList.begin(); iter != permutationList.end(); iter++ )
	{
		Permutation* permutation = *iter;
		set.AddMember( ( uint )permutation->map->size() );
	}

	uint order = set.CalcLCM();

	Delete( permutationList );

	return order;
}

void Permutation::SetCopy( const Permutation& permutation )
{
	permutation.GetCopy( *this );
}

void Permutation::GetCopy( Permutation& permutation ) const
{
	permutation.map->clear();

	for( Map::const_iterator iter = map->cbegin(); iter != map->cend(); iter++ )
		permutation.map->insert( std::pair< uint, uint >( iter->first, iter->second ) );
}

bool Permutation::SetInverse( const Permutation& permutation )
{
	return permutation.GetInverse( *this );
}

bool Permutation::GetInverse( Permutation& permutation ) const
{
	Prune();

	permutation.map->clear();

	NaturalNumberSet inputSet, outputSet;

	for( Map::const_iterator iter = map->cbegin(); iter != map->cend(); iter++ )
	{
		inputSet.AddMember( iter->first );
		inputSet.AddMember( iter->second );
	}

	for( NaturalNumberSet::UintSet::iterator iter = inputSet.set.begin(); iter != inputSet.set.end(); iter++ )
	{
		uint input = *iter;
		uint output = Evaluate( input );

		if( outputSet.IsMember( output ) )
			return false;
		else
			outputSet.AddMember( output );

		permutation.map->insert( std::pair< uint, uint >( output, input ) );
	}

	return true;
}

void Permutation::Multiply( const Permutation& permutationA, const Permutation& permutationB )
{
	PruneInfo infoA, infoB;

	permutationA.Prune( &infoA );
	permutationB.Prune( &infoB );

	uint smallestInput = ( infoA.smallestInput < infoB.smallestInput ) ? infoA.smallestInput : infoB.smallestInput;
	uint largestInput = ( infoA.largestInput > infoB.largestInput ) ? infoA.largestInput : infoB.largestInput;

	map->clear();

	for( uint i = smallestInput; i <= largestInput; i++ )
	{
		uint j = permutationA.Evaluate(i);
		uint k = permutationB.Evaluate(j);

		if( i != k )
			map->insert( std::pair< uint, uint >( i, k ) );
	}
}

void Permutation::MultiplyOnRight( const Permutation& permutation )
{
	Permutation product;
	product.Multiply( *this, permutation );
	SetCopy( product );
}

void Permutation::MultiplyOnleft( const Permutation& permutation )
{
	Permutation product;
	product.Multiply( permutation, *this );
	SetCopy( product );
}

bool Permutation::Factor( PermutationList& permutationList ) const
{
	if( !IsValid() )
		return false;

	NaturalNumberSet inputSet;

	for( Map::iterator iter = map->begin(); iter != map->end(); iter++ )
		inputSet.AddMember( iter->first );

	while( inputSet.Cardinality() > 0 )
	{
		NaturalNumberSet::UintSet::iterator iter = inputSet.set.begin();
		uint i = *iter;
		inputSet.set.erase( iter );

		Permutation* cycle = new Permutation();

		int j = i;

		do
		{
			uint k = Evaluate(j);
			cycle->map->insert( std::pair< uint, uint >( j, k ) );
			j = k;
			inputSet.RemoveMember(j);
		}
		while( j != i );

		if( cycle->IsIdentity() )
			delete cycle;
		else
			permutationList.push_back( cycle );
	}

	return true;
}

void Permutation::Print( std::ostream& ostream, bool isCycle /*= false*/ ) const
{
	if( !isCycle )
	{
		PermutationList permutationList;
		if( !Factor( permutationList ) )
			ostream << "(invalid)";
		else if( permutationList.size() == 0 )
			ostream << "(1)";
		else
			for( PermutationList::iterator iter = permutationList.begin(); iter != permutationList.end(); iter++ )
				( *iter )->Print( ostream, true );

		Delete( permutationList );
	}
	else
	{
		PruneInfo info;
		Prune( &info );

		ostream << "(";

		uint i = info.smallestInput;
		do
		{
			if( i != info.smallestInput )
				ostream << ",";
			ostream << i;
			i = Evaluate(i);
		}
		while( i != info.smallestInput );

		ostream << ")";
	}
}

void Permutation::GetUnstableSet( NaturalNumberSet& unstableSet ) const
{
	Prune();

	unstableSet.RemoveAllMembers();

	for( Map::const_iterator iter = map->cbegin(); iter != map->cend(); iter++ )
		unstableSet.AddMember( iter->first );
}

void Permutation::Prune( PruneInfo* info /*= nullptr*/ ) const
{
	const_cast< Permutation* >( this )->Prune( info );
}

void Permutation::Prune( PruneInfo* info /*= nullptr*/ )
{
	if( info )
	{
		info->largestInput = 0;
		info->smallestInput = -1;
		info->largestOutput = 0;
		info->smallestOutput = -1;
	}

	Map::iterator iter = map->begin();

	while( iter != map->end() )
	{
		Map::iterator nextIter = iter;
		nextIter++;

		if( iter->first == iter->second )
			map->erase( iter );
		else if( info )
		{
			if( iter->first > info->largestInput )
				info->largestInput = iter->first;
			if( iter->first < info->smallestInput )
				info->smallestInput = iter->first;
			if( iter->second > info->largestOutput )
				info->largestOutput = iter->second;
			if( iter->second < info->smallestOutput )
				info->smallestOutput = iter->second;
		}

		iter = nextIter;
	}
}

/*static*/ void Permutation::Delete( PermutationList& permutationList )
{
	while( permutationList.size() > 0 )
	{
		PermutationList::iterator iter = permutationList.begin();
		Permutation* permutation = *iter;
		delete permutation;
		permutationList.erase( iter );
	}
}

// Permutation.cpp