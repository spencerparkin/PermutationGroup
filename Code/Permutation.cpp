// Permutation.cpp

#include "Permutation.h"
#include "NaturalNumberSet.h"
#include <sstream>

//------------------------------------------------------------------------------------------
//                                        Permutation
//------------------------------------------------------------------------------------------

Permutation::Permutation( void )
{
	word = nullptr;

	DefineIdentity();
}

Permutation::Permutation( const Permutation& permutation )
{
	word = nullptr;

	SetCopy( permutation );
}

/*virtual*/ Permutation::~Permutation( void )
{
	delete word;
}

uint Permutation::Evaluate( uint input ) const
{
	uint output = input;
	if( input < MAX_MAP_SIZE )
		output = map[ input ];
	return output;
}

void Permutation::DefineIdentity( void )
{
	for( int i = 0; i < MAX_MAP_SIZE; i++ )
		map[i] = i;
}

bool Permutation::Define( uint input, uint output )
{
	if( input >= MAX_MAP_SIZE || output >= MAX_MAP_SIZE )
		return false;
	map[ input ] = output;
	return true;
}

std::size_t Permutation::CalcHash( void ) const
{
	std::stringstream stream;

	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		if( map[i] != i )
			stream << i << map[i];

	std::string str = stream.str();
	std::size_t hash = std::hash< std::string >{}( str );

	return hash;
}

bool Permutation::IsValid( void ) const
{
	NaturalNumberSet set;

	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
	{
		uint j = map[i];
		if( j >= MAX_MAP_SIZE )
			return false;
		if( set.IsMember(j) )
			return false;
		set.AddMember(j);
	}

	return true;
}

bool Permutation::IsEven( void ) const
{
	PermutationList permutationList;
	Factor( permutationList );

	uint total = 0;
	for( PermutationList::iterator iter = permutationList.begin(); iter != permutationList.end(); iter++ )
	{
		// An N-cycle can be written as N-1 transpositions.
		const Permutation& permutation = *iter;
		uint cycleOrder = permutation.CycleOrder();
		total += cycleOrder - 1;
	}

	return( total % 2 == 0 ? true : false );
}

bool Permutation::IsOdd( void ) const
{
	return !IsEven();
}

bool Permutation::IsIdentity( void ) const
{
	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		if( map[i] != i )
			return false;
	return true;
}

bool Permutation::IsEqualTo( const Permutation& permutation ) const
{
	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		if( map[i] != permutation.map[i] )
			return false;
	return true;
}

bool Permutation::CommutesWith( const Permutation& permutation ) const
{
	Permutation productA, productB;

	productA.Multiply( *this, permutation );
	productB.Multiply( permutation, *this );

	return productA.IsEqualTo( productB );
}

uint Permutation::CycleOrder( void ) const
{
	uint cycleOrder = 0;
	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		if( map[i] != i )
			cycleOrder++;
	return cycleOrder;
}

uint Permutation::Order( void ) const
{
	PermutationList permutationList;
	Factor( permutationList );

	NaturalNumberSet set;

	for( PermutationList::iterator iter = permutationList.begin(); iter != permutationList.end(); iter++ )
	{
		const Permutation& permutation = *iter;
		set.AddMember( permutation.CycleOrder() );
	}

	uint order = set.CalcLCM();

	return order;
}

void Permutation::SetCopy( const Permutation& permutation )
{
	permutation.GetCopy( *this );

	if( permutation.word )
	{
		delete word;
		word = new ElementList;

		for( ElementList::const_iterator iter = permutation.word->cbegin(); iter != permutation.word->cend(); iter++ )
			word->push_back( *iter );
	}
}

void Permutation::GetCopy( Permutation& permutation ) const
{
	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		permutation.map[i] = map[i];
}

bool Permutation::SetInverse( const Permutation& permutation )
{
	return permutation.GetInverse( *this );
}

bool Permutation::GetInverse( Permutation& permutation ) const
{
	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
	{
		uint j = map[i];
		if( j >= MAX_MAP_SIZE )
			return false;

		permutation.map[j] = i;
	}

	if( word )
	{
		delete permutation.word;
		permutation.word = new ElementList;

		for( ElementList::const_iterator iter = word->cbegin(); iter != word->cend(); iter++ )
		{
			const Element& element = *iter;
			Element invElement;
			invElement.name = element.name;
			invElement.exponent = -element.exponent;
			permutation.word->push_front( invElement );
		}
	}

	return true;
}

void Permutation::Multiply( const Permutation& permutationA, const Permutation& permutationB )
{
	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		map[i] = permutationB.Evaluate( permutationA.Evaluate(i) );

	if( permutationA.word && permutationB.word )
	{
		if( !word )
			word = new ElementList;
		else
			word->clear();

		for( ElementList::const_iterator iter = permutationA.word->cbegin(); iter != permutationA.word->cend(); iter++ )
			word->push_back( *iter );

		for( ElementList::const_iterator iter = permutationB.word->cbegin(); iter != permutationB.word->cend(); iter++ )
			word->push_back( *iter );
	}
}

void Permutation::MultiplyOnRight( const Permutation& permutation )
{
	Permutation product;
	product.Multiply( *this, permutation );
	SetCopy( product );
}

void Permutation::MultiplyOnLeft( const Permutation& permutation )
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
	GetUnstableSet( inputSet );

	while( inputSet.Cardinality() > 0 )
	{
		NaturalNumberSet::UintSet::iterator iter = inputSet.set.begin();
		uint i = *iter;
		inputSet.set.erase( iter );

		Permutation cycle;

		int j = i;

		do
		{
			uint k = Evaluate(j);
			cycle.Define( j, k );
			j = k;
			inputSet.RemoveMember(j);
		}
		while( j != i );

		if( !cycle.IsIdentity() )
			permutationList.push_back( cycle );
	}

	return true;
}

void Permutation::Print( std::ostream& ostream, bool isCycle /*= false*/ ) const
{
	if( !isCycle )
	{
		std::string name = GetName();
		ostream << name << " = ";

		PermutationList permutationList;
		if( !Factor( permutationList ) )
			ostream << "(invalid)";
		else if( permutationList.size() == 0 )
			ostream << "(1)";
		else
			for( PermutationList::iterator iter = permutationList.begin(); iter != permutationList.end(); iter++ )
				( *iter ).Print( ostream, true );

		ostream << "\n";
	}
	else
	{
		uint i;
		for( i = 0; i < MAX_MAP_SIZE; i++ )
			if( map[i] != i )
				break;
		
		if( i < MAX_MAP_SIZE )
		{
			ostream << "(";

			uint j = i;
			do
			{
				ostream << j;
				j = Evaluate(j);
				if( j != i )
					ostream << ",";
			}
			while( j != i );

			ostream << ")";
		}
	}
}

void Permutation::GetUnstableSet( NaturalNumberSet& unstableSet ) const
{
	unstableSet.RemoveAllMembers();

	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		if( map[i] != i )
			unstableSet.AddMember(i);
}

void Permutation::GetStableSet( NaturalNumberSet& stableSet ) const
{
	stableSet.RemoveAllMembers();

	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		if( map[i] == i )
			stableSet.AddMember(i);
}

bool Permutation::operator==( const Permutation& permutation ) const
{
	return IsEqualTo( permutation );
}

void Permutation::operator=( const Permutation& permutation )
{
	permutation.GetCopy( *this );
}

void Permutation::SetName( const std::string& name )
{
	if( word )
	{
		delete word;
		word = nullptr;
	}

	word = new ElementList;

	Element element;
	element.name = name;
	element.exponent = 1;
	word->push_back( element );
}

std::string Permutation::GetName( void ) const
{
	if( !word )
		return "anonymous";

	std::stringstream stream;

	if( word->size() == 0 )
		stream << "identity";
	else
	{
		for( ElementList::const_iterator iter = word->cbegin(); iter != word->cend(); iter++ )
		{
			const Element& element = *iter;
			stream << element.name;
			if( element.exponent != 1 )
				stream << "^{" << element.exponent << "}";
		}
	}

	return stream.str();
}

// Permutation.cpp