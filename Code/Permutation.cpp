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

bool Permutation::Stabilizes( const NaturalNumberSet& set ) const
{
	for( NaturalNumberSet::UintSet::const_iterator iter = set.set.cbegin(); iter != set.set.cend(); iter++ )
	{
		uint inputPoint = *iter;
		uint outputPoint = Evaluate( inputPoint );
		if( inputPoint != outputPoint )
			return false;
	}

	return true;
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

bool Permutation::DefineCycle( uint a, uint b )
{
	if( !Define( a, b ) )
		return false;
	if( !Define( b, a ) )
		return false;
	return true;
}

bool Permutation::DefineCycle( uint a, uint b, uint c )
{
	if( !Define( a, b ) )
		return false;
	if( !Define( b, c ) )
		return false;
	if( !Define( c, a ) )
		return false;
	return true;
}

bool Permutation::DefineCycle( uint a, uint b, uint c, uint d )
{
	if( !Define( a, b ) )
		return false;
	if( !Define( b, c ) )
		return false;
	if( !Define( c, d ) )
		return false;
	if( !Define( d, a ) )
		return false;
	return true;
}

bool Permutation::DefineCycle( uint a, uint b, uint c, uint d, uint e )
{
	if( !Define( a, b ) )
		return false;
	if( !Define( b, c ) )
		return false;
	if( !Define( c, d ) )
		return false;
	if( !Define( d, e ) )
		return false;
	if( !Define( e, a ) )
		return false;
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
}

void Permutation::GetCopy( Permutation& permutation ) const
{
	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		permutation.map[i] = map[i];

	delete permutation.word;
	permutation.word = nullptr;

	if( word )
	{
		permutation.word = new ElementList;
		for( ElementList::const_iterator iter = word->cbegin(); iter != word->cend(); iter++ )
			permutation.word->push_back( *iter );
	}
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
		if( name.length() > 0 )
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
		stream << "";
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

/*static*/ bool Permutation::LexigraphicCompare( const Permutation& permLeft, const Permutation& permRight )
{
	uint leftSize = permLeft.word ? permLeft.word->size() : 0;
	uint rightSize = permRight.word ? permRight.word->size() : 0;

	if( leftSize < rightSize )
		return true;
	else if( leftSize > rightSize )
		return false;

	std::string leftKey = permLeft.MakeKeyForLexigraphicCompare();
	std::string rightKey = permRight.MakeKeyForLexigraphicCompare();

	int cmp = strcmp( leftKey.c_str(), rightKey.c_str() );
	if( cmp < 0 )
		return true;
	return false;
}

std::string Permutation::MakeKeyForLexigraphicCompare( void ) const
{
	if( !word || word->size() == 0 )
		return "";

	std::stringstream stream;

	for( ElementList::const_iterator iter = word->cbegin(); iter != word->cend(); iter++ )
	{
		const Element& element = *iter;
		stream << element.name;
		if( element.exponent > 0 )
			stream << "p";
		else
			stream << "m";
		stream << abs( element.exponent );
	}

	return stream.str();
}

bool Permutation::GetToJsonValue( rapidjson::Value& value, rapidjson::Document::AllocatorType& allocator ) const
{
	if( word )
	{
		rapidjson::Value wordArray( rapidjson::kArrayType );

		for( ElementList::const_iterator iter = word->cbegin(); iter != word->cend(); iter++ )
		{
			const Element& element = *iter;
						
			rapidjson::Value stringValue( rapidjson::kStringType );
			stringValue.SetString( element.name.c_str(), allocator );

			rapidjson::Value elementValue( rapidjson::kObjectType );
			elementValue.AddMember( "name", stringValue, allocator );
			elementValue.AddMember( "exponent", element.exponent, allocator );
			wordArray.PushBack( elementValue, allocator );
		}

		value.AddMember( "word", wordArray, allocator );
	}

	// We could certainly store this more compactly.
	rapidjson::Value mapArray( rapidjson::kArrayType );
	for( uint i = 0; i < MAX_MAP_SIZE; i++ )
		mapArray.PushBack( map[i], allocator );

	value.AddMember( "map", mapArray, allocator );

	return true;
}

bool Permutation::SetFromJsonValue( /*const*/ rapidjson::Value& value )
{
	delete word;
	word = nullptr;

	if( value.HasMember( "word" ) && value[ "word" ].IsArray() )
	{
		word = new ElementList;

		rapidjson::Value wordArray = value[ "word" ].GetArray();

		for( uint i = 0; i < wordArray.Size(); i++ )
		{
			rapidjson::Value elementValue = wordArray[i].GetObject();

			Element element;
			element.name = elementValue[ "name" ].GetString();
			element.exponent = elementValue[ "exponent" ].GetInt();

			word->push_back( element );
		}
	}

	if( !value.HasMember( "map" ) )
		return false;

	DefineIdentity();

	rapidjson::Value mapArray = value[ "map" ].GetArray();
	if( mapArray.Size() > MAX_MAP_SIZE )
		return false;

	for( uint i = 0; i < mapArray.Size(); i++ )
		map[i] = mapArray[i].GetInt();

	return true;
}

/*static*/ bool Permutation::LoadPermutationSet( PermutationSet& permutationSet, /*const*/ rapidjson::Value& arrayValue )
{
	permutationSet.clear();

	for( uint i = 0; i < arrayValue.Size(); i++ )
	{
		rapidjson::Value permutationValue = arrayValue[i].GetObject();

		Permutation permutation;
		if( !permutation.SetFromJsonValue( permutationValue ) )
			return false;

		permutationSet.insert( permutation );
	}

	return true;
}

/*static*/ bool Permutation::SavePermutationSet( const PermutationSet& permutationSet, rapidjson::Value& arrayValue, rapidjson::Document::AllocatorType& allocator )
{
	for( PermutationSet::const_iterator iter = permutationSet.cbegin(); iter != permutationSet.cend(); iter++ )
	{
		rapidjson::Value permutationValue( rapidjson::kObjectType );
		if( !( *iter ).GetToJsonValue( permutationValue, allocator ) )
			return false;

		arrayValue.PushBack( permutationValue, allocator );
	}

	return true;
}

// This doesn't do everything that could possibly be done to compress a word.
// It might be worth looking into what else can be done.
bool Permutation::CompressWord( const CompressInfo& compressInfo )
{
	if( !word )
		return false;

	typedef std::vector< Element > ElementArray;
	ElementArray elementArray;

	for( ElementList::iterator iter = word->begin(); iter != word->end(); iter++ )
		elementArray.push_back( *iter );

	bool combinedElements;

	do
	{
		combinedElements = false;

		// This isn't optimal, but it will work.
		for( uint i = 0; i < elementArray.size() && !combinedElements; i++ )
		{
			Element& element = elementArray[i];

			for( uint j = 0; j < 2 && !combinedElements; j++ )
			{
				uint k = ( j == 0 ) ? ( i - 1 ) : ( i + 1 );
				while( k >= 0 && k < elementArray.size() )
				{
					Element& otherElement = elementArray[k];

					if( element.name == otherElement.name )
					{
						element.exponent += otherElement.exponent;

						elementArray.erase( elementArray.begin() + k );
						if( element.exponent == 0 )
							elementArray.erase( elementArray.begin() + i );

						combinedElements = true;
						break;
					}

					if( !compressInfo.ElementsCommute( element, otherElement ) )
						break;

					k = ( j == 0 ) ? ( k - 1 ) : ( k + 1 );
				}
			}
		}
	}
	while( combinedElements );

	word->clear();

	for( uint i = 0; i < elementArray.size(); i++ )
	{
		Element& element = elementArray[i];

		uint order = compressInfo.ElementOrder( element );

		while( abs( element.exponent ) >= ( signed )order )
		{
			if( element.exponent > 0 )
				element.exponent -= order;
			else
				element.exponent += order;
		}

		if( element.exponent != 0 )
		{
			if( element.exponent > 0 )
			{
				if( -( element.exponent - int( order ) ) < element.exponent )
					element.exponent -= order;
			}
			else
			{
				if( element.exponent + int( order ) < -element.exponent )
					element.exponent += order;
			}

			word->push_back( element );
		}
	}

	return true;
}

bool CompressInfo::ElementsCommute( const Element& elementA, const Element& elementB ) const
{
	if( commuteMap.size() == 0 )
	{
		// Half of this is redundant, but that's fine.
		for( PermutationMap::const_iterator iterA = permutationMap.cbegin(); iterA != permutationMap.cend(); iterA++ )
		{
			const Permutation& permA = iterA->second;

			for( PermutationMap::const_iterator iterB = permutationMap.cbegin(); iterB != permutationMap.cend(); iterB++ )
			{
				const Permutation& permB = iterB->second;

				Permutation productAB;
				productAB.Multiply( permA, permB );

				Permutation productBA;
				productBA.Multiply( permB, permA );

				bool commute = productAB.IsEqualTo( productBA );
				std::string key = iterA->first + "*" + iterB->first;
				commuteMap.insert( std::pair< std::string, bool >( key, commute ) );
			}
		}
	}

	CommuteMap::iterator iter = commuteMap.find( elementA.name + "*" + elementB.name );
	if( iter == commuteMap.end() )
		return false;

	return iter->second;
}

uint CompressInfo::ElementOrder( const Element& element ) const
{
	if( orderMap.size() == 0 )
	{
		for( PermutationMap::const_iterator iter = permutationMap.cbegin(); iter != permutationMap.cend(); iter++ )
		{
			const Permutation& perm = iter->second;
			uint order = perm.Order();
			orderMap.insert( std::pair< std::string, uint >( iter->first, order ) );
		}
	}

	OrderMap::iterator iter = orderMap.find( element.name );
	if( iter == orderMap.end() )
		return -1;

	return iter->second;
}

// Permutation.cpp