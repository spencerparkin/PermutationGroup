// NaturalNumberSet.cpp

#include "NaturalNumberSet.h"
#include <ostream>

//------------------------------------------------------------------------------------------
//                                    NaturalNumberSet
//------------------------------------------------------------------------------------------

NaturalNumberSet::NaturalNumberSet( void )
{
}

/*virtual*/ NaturalNumberSet::~NaturalNumberSet( void )
{
}

bool NaturalNumberSet::IsEmpty( void ) const
{
	return( set.size() == 0 ? true : false );
}

bool NaturalNumberSet::IsMember( uint x ) const
{
	UintSet::iterator iter = set.find(x);
	if( iter == set.end() )
		return false;
	return true;
}

void NaturalNumberSet::AddMember( uint x )
{
	set.insert(x);
}

void NaturalNumberSet::RemoveMember( uint x )
{
	set.erase(x);
}

void NaturalNumberSet::RemoveAllMembers( void )
{
	set.clear();
}

uint NaturalNumberSet::Cardinality( void ) const
{
	return ( uint )set.size();
}

void NaturalNumberSet::Copy( const NaturalNumberSet& set, bool clear /*= true*/ )
{
	if( clear )
		this->set.clear();
	for( UintSet::const_iterator iter = set.set.cbegin(); iter != set.set.cend(); iter++ )
		this->set.insert( *iter );
}

void NaturalNumberSet::Union( const NaturalNumberSet& setA, const NaturalNumberSet& setB )
{
	Copy( setA );
	Copy( setB, false );
}

void NaturalNumberSet::Intersect( const NaturalNumberSet& setA, const NaturalNumberSet& setB )
{
	set.clear();

	for( UintSet::const_iterator iter = setA.set.cbegin(); iter != setA.set.cend(); iter++ )
		if( setB.IsMember( *iter ) )
			set.insert( *iter );
}

void NaturalNumberSet::GenerateDivisorsOf( uint x )
{
	set.clear();

	for( uint d = 1; d <= x / 2; d++ )
		if( x % d == 0 )
			set.insert(d);
}

uint NaturalNumberSet::CalcLCM( void ) const
{
	if( Cardinality() == 0 )
		return 0;
	else if( Cardinality() == 1 )
		return *set.begin();
	else if( Cardinality() == 2 )
	{
		UintSet::iterator iter = set.begin();
		iter++;
		return Lcm( *set.begin(), *iter );
	}

	NaturalNumberSet reducedSet;
	reducedSet.Copy( *this );
	reducedSet.RemoveMember( *set.begin() );
	return Lcm( reducedSet.CalcLCM(), *set.begin() );
}

uint NaturalNumberSet::CalcGCD( void ) const
{
	if( Cardinality() == 0 )
		return 0;
	else if( Cardinality() == 1 )
		return *set.begin();
	else if( Cardinality() == 2 )
	{
		UintSet::iterator iter = set.begin();
		iter++;
		return Gcd( *set.begin(), *iter );
	}

	NaturalNumberSet reducedSet;
	reducedSet.Copy( *this );
	reducedSet.RemoveMember( *set.begin() );
	return Lcm( reducedSet.CalcGCD(), *set.begin() );
}

/*static*/ uint NaturalNumberSet::Lcm( uint a, uint b )
{
	return( a * b / Gcd( a, b ) );
}

/*static*/ uint NaturalNumberSet::Gcd( uint a, uint b )
{
	// This is Euclid's algorithm.
	if( b == 0 )
		return a;
	return Gcd( b, a % b );
}

uint NaturalNumberSet::Max( void ) const
{
	uint maxMember = 0;
	for( UintSet::const_iterator iter = set.cbegin(); iter != set.cend(); iter++ )
		if( *iter > maxMember )
			maxMember = *iter;
	return maxMember;
}

uint NaturalNumberSet::Min( void ) const
{
	uint minMember = -1;
	for( UintSet::const_iterator iter = set.cbegin(); iter != set.cend(); iter++ )
		if( *iter < minMember )
			minMember = *iter;
	return minMember;
}

bool NaturalNumberSet::IsSubsetOf( const NaturalNumberSet& givenSet ) const
{
	for( UintSet::const_iterator iter = set.cbegin(); iter != set.cend(); iter++ )
		if( !givenSet.IsMember( *iter ) )
			return false;
	return true;
}

bool NaturalNumberSet::IsSupsetOf( const NaturalNumberSet& givenSet ) const
{
	return givenSet.IsSubsetOf( *this );
}

void NaturalNumberSet::Print( std::ostream& ostream ) const
{
	ostream << "{";
	UintSet::const_iterator iter = set.cbegin();
	while( iter != set.cend() )
	{
		ostream << *iter;
		UintSet::const_iterator nextIter = iter;
		nextIter++;
		if( nextIter != set.cend() )
			ostream << ",";
		iter = nextIter;
	}
	ostream << "}\n";
}

// NaturalNumberSet.cpp