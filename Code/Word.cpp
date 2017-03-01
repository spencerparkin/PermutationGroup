// Word.cpp

#include "Word.h"

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

bool WordCompressor::Compress( Word& word )
{
	//...
	return true;
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