// Word.h

#pragma once

#include <string>
#include <list>

typedef unsigned int uint;

//------------------------------------------------------------------------------------------
//                                          Term
//------------------------------------------------------------------------------------------

struct Term
{
	std::string name;
	int exponent;

	void GetInverse( Term& term ) const;
};

typedef std::list< Term > TermList;

//------------------------------------------------------------------------------------------
//                                       WordCompressor
//------------------------------------------------------------------------------------------

class Word;

class WordCompressor
{
public:

	WordCompressor( void );
	virtual ~WordCompressor( void );

	bool Compress( Word& word );

	virtual bool TermsCommute( const Term& termA, const Term& termB ) = 0;
	virtual uint TermOrder( const Term& term ) = 0;
};

//------------------------------------------------------------------------------------------
//                                           Word
//------------------------------------------------------------------------------------------

// A word is a sequence/product of terms, each being a symbol raised to an integral power.
class Word
{
public:

	Word( void );
	virtual ~Word( void );

	void SetName( const std::string& name );
	void Multiply( const Word& wordA, const Word& wordB );
	void MultiplyOnRight( const Word& word );
	void MultiplyOnLeft( const Word& word );
	void SetInverse( const Word& word );
	void GetInverse( Word& word ) const;
	void SetCopy( const Word& word );
	void GetCopy( Word& word ) const;
	bool Compress( WordCompressor& compressor );
	void Print( std::ostream& ostream ) const;

	TermList termList;
};

// Word.h