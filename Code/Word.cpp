// Word.cpp

#include "Word.h"
#include "ElementSet.h"

//------------------------------------------------------------------------------------------
//                                          Term
//------------------------------------------------------------------------------------------

void Term::GetInverse( Term& term ) const
{
	term.name = name;
	term.exponent = -exponent;
}

//------------------------------------------------------------------------------------------
//                                       WordCompressor
//------------------------------------------------------------------------------------------

WordCompressor::WordCompressor( void )
{
}

/*virtual*/ WordCompressor::~WordCompressor( void )
{
}

bool WordCompressor::Compress( ElementSet& elementSet )
{
	for( ElementList::iterator iter = elementSet.elementList.begin(); iter != elementSet.elementList.end(); iter++ )
		if( !Compress( *( *iter )->GetWord() ) )
			return false;
	return true;
}

bool WordCompressor::Compress( Word& word )
{
	bool combined = false;

	do
	{
		combined = false;

		for( TermList::iterator iter = word.termList.begin(); iter != word.termList.end(); iter++ )
		{
			TermList::iterator foundIter = FindCombineTerm( iter, word.termList );

			if( foundIter != word.termList.end() )
			{
				Term& termA = *iter;
				Term& termB = *foundIter;

				Term newTerm;
				newTerm.name = termA.name;
				newTerm.exponent = termA.exponent = termB.exponent;
				
				if( newTerm.exponent != 0 )
					word.termList.insert( iter, newTerm );

				word.termList.erase( iter );
				word.termList.erase( foundIter );

				combined = true;
				break;
			}
		}
	}
	while( combined );

	for( TermList::iterator iter = word.termList.begin(); iter != word.termList.end(); iter++ )
	{
		Term& term = *iter;

		uint order = TermOrder( term );

		if( term.exponent > 0 )
			term.exponent %= order;
		else
			term.exponent = -int( -term.exponent % order );
	}

	return true;
}

TermList::iterator WordCompressor::FindCombineTerm( TermList::iterator& iter, TermList& termList )
{
	TermList::iterator foundIter = iter;

	do
	{
		foundIter++;
		if( foundIter != termList.end() && CanCombineTerms( *iter, *foundIter ) )
			return foundIter;
	}
	while( foundIter != termList.end() && TermsCommute( *iter, *foundIter ) );

	foundIter = iter;

	while( foundIter != termList.begin() )
	{
		foundIter--;
		if( CanCombineTerms( *iter, *foundIter ) )
			return foundIter;
		if( !TermsCommute( *iter, *foundIter ) )
			break;
	}

	return termList.end();
}

/*virtual*/ bool WordCompressor::CanCombineTerms( const Term& termA, const Term& termB )
{
	if( termA.name != termB.name )
		return false;

	return true;
}

//------------------------------------------------------------------------------------------
//                                  ConfiguredWordCompressor
//------------------------------------------------------------------------------------------

ConfiguredWordCompressor::ConfiguredWordCompressor( void )
{
}

/*virtual*/ ConfiguredWordCompressor::~ConfiguredWordCompressor( void )
{
}

bool ConfiguredWordCompressor::Configure( const ElementSet& elementSet )
{
	orderMap.clear();

	for( ElementList::const_iterator iter = elementSet.elementList.cbegin(); iter != elementSet.elementList.cend(); iter++ )
	{
		const Element* element = *iter;
		std::string name = element->Name();
		uint order = element->Order();
		orderMap.insert( std::pair< std::string, uint >( name, order ) );
	}

	commuteMap.clear();

	for( ElementList::const_iterator iterA = elementSet.elementList.cbegin(); iterA != elementSet.elementList.cend(); iterA++ )
	{
		const Element* elementA = *iterA;

		for( ElementList::const_iterator iterB = iterA; iterB != elementSet.elementList.cend(); iterB++ )
		{
			const Element* elementB = *iterB;

			Element* productAB = elementSet.Multiply( elementA, elementB );
			Element* productBA = elementSet.Multiply( elementB, elementA );

			bool commutes = elementSet.AreEqual( productAB, productBA );

			std::string key = MakeCommuteMapKey( elementA->Name(), elementB->Name() );

			commuteMap.insert( std::pair< std::string, bool >( key, commutes ) );
		}
	}

	return true;
}

std::string ConfiguredWordCompressor::MakeCommuteMapKey( const std::string& nameA, const std::string& nameB )
{
	std::string key;
	if( nameA.compare( nameA ) < 0 )
		key = nameA + nameB;
	else
		key = nameB + nameA;
	return key;
}

/*virtual*/ bool ConfiguredWordCompressor::TermsCommute( const Term& termA, const Term& termB )
{
	std::string key = MakeCommuteMapKey( termA.name, termB.name );
	CommuteMap::iterator iter = commuteMap.find( key );
	if( iter == commuteMap.end() )
		return false;
	return iter->second;
}

/*virtual*/ uint ConfiguredWordCompressor::TermOrder( const Term& term )
{
	OrderMap::iterator iter = orderMap.find( term.name );
	if( iter == orderMap.end() )
		return 0;
	return iter->second;
}

//------------------------------------------------------------------------------------------
//                                           Word
//------------------------------------------------------------------------------------------

Word::Word( void )
{
}

/*virtual*/ Word::~Word( void )
{
}

void Word::Clear( void )
{
	termList.clear();
}

void Word::SetName( const std::string& name )
{
	termList.clear();

	Term term;
	term.name = name;
	term.exponent = 1;
	termList.push_back( term );
}

void Word::Multiply( const Word& wordA, const Word& wordB )
{
	termList.clear();

	for( TermList::const_iterator iter = wordA.termList.cbegin(); iter != wordA.termList.cend(); iter++ )
		termList.push_back( *iter );

	for( TermList::const_iterator iter = wordB.termList.cbegin(); iter != wordB.termList.cend(); iter++ )
		termList.push_back( *iter );
}

void Word::MultiplyOnRight( const Word& word )
{
	for( TermList::const_iterator iter = word.termList.cbegin(); iter != word.termList.cend(); iter++ )
		termList.push_back( *iter );
}

void Word::MultiplyOnLeft( const Word& word )
{
	for( TermList::const_reverse_iterator iter = word.termList.crbegin(); iter != word.termList.crend(); iter-- )
		termList.push_front( *iter );
}

void Word::SetInverse( const Word& word )
{
	word.GetInverse( *this );
}

void Word::GetInverse( Word& word ) const
{
	word.termList.clear();

	for( TermList::const_iterator iter = termList.cbegin(); iter != termList.cend(); iter++ )
	{
		const Term& term = *iter;
		Term invTerm;
		term.GetInverse( invTerm );
		word.termList.push_front( invTerm );
	}
}

void Word::SetCopy( const Word& word )
{
	word.GetCopy( *this );
}

void Word::GetCopy( Word& word ) const
{
	word.termList.clear();

	for( TermList::const_iterator iter = termList.cbegin(); iter != termList.cend(); iter++ )
		word.termList.push_back( *iter );
}

bool Word::Compress( WordCompressor& compressor )
{
	return compressor.Compress( *this );
}

void Word::Print( std::ostream& ostream ) const
{
	if( termList.size() == 0 )
		ostream << "e";
	else
	{
		for( TermList::const_iterator iter = termList.cbegin(); iter != termList.cend(); iter++ )
		{
			const Term& term = *iter;
			ostream << term.name;
			if( term.exponent != 1 )
				ostream << "^{" << term.exponent << "}";
		}
	}
}

// Word.cpp