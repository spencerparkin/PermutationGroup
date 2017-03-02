// Word.h

#pragma once

#include <string>
#include <list>
#include "Permutation.h"

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
class ElementSet;

class WordCompressor
{
public:

	WordCompressor( void );
	virtual ~WordCompressor( void );

	bool Compress( ElementSet& elementSet );
	bool Compress( Word& word );

	TermList::iterator FindCombineTerm( TermList::iterator& iter, TermList& termList );
	
	virtual bool CanCombineTerms( const Term& termA, const Term& termB );
	virtual bool TermsCommute( const Term& termA, const Term& termB ) = 0;
	virtual uint TermOrder( const Term& term ) = 0;
};

//------------------------------------------------------------------------------------------
//                                  ConfiguredWordCompressor
//------------------------------------------------------------------------------------------

class ConfiguredWordCompressor : public WordCompressor
{
public:

	ConfiguredWordCompressor( void );
	virtual ~ConfiguredWordCompressor( void );

	bool Configure( const ElementSet& elementSet );

	virtual bool TermsCommute( const Term& termA, const Term& termB ) override;
	virtual uint TermOrder( const Term& term ) override;

	std::string MakeCommuteMapKey( const std::string& nameA, const std::string& nameB );

	typedef std::map< std::string, uint > OrderMap;
	OrderMap orderMap;

	typedef std::map< std::string, bool > CommuteMap;
	CommuteMap commuteMap;
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

	void Clear( void );
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