// StabilizerChain.cpp

#include "StabilizerChain.h"
#include <memory>

//------------------------------------------------------------------------------------------
//                                   StabilizerChainGroup
//------------------------------------------------------------------------------------------

StabilizerChainGroup::StabilizerChainGroup( void )
{
	subGroup = nullptr;
}

StabilizerChainGroup::~StabilizerChainGroup( void )
{
	delete subGroup;
}

bool StabilizerChainGroup::Generate( const NaturalNumberSet& domainSet, WordCompressor* wordCompressor /*= nullptr*/, std::ostream* ostream /*= nullptr*/ )
{
	factorGroup.unstableSet.Copy( domainSet );
	NaturalNumberSet::UintSet::iterator iter = factorGroup.unstableSet.set.begin();
	factorGroup.unstableSet.RemoveMember( *iter );

	if( wordCompressor )
	{
		if( ostream )
			*ostream << "Compressing words in generator set...\n";

		wordCompressor->Compress( generatorSet );
	}

	if( ostream )
	{
		*ostream << "Generators for subgroup in stabilizer chain...\n";

		for( uint i = 0; i < generatorSet.elementArray.size(); i++ )
		{
			const Element* generator = generatorSet.elementArray[i];
			if( ostream )
				generator->Print( *ostream );
		}

		*ostream << "Generating factor group for unstable set...\n";
		factorGroup.unstableSet.Print( *ostream );
		*ostream << "\n";
	}

	// CRITICAL FAIL!!!!: We can't create a transversal like this, because the stabilizer is not necessarily a normal subgroup!
	//                    If it was a normal subgroup, then we can compute the generators for the factor group using the generators for the group,
	//                    but the stabilizer is not generally normal!  Trying to create a factor group out of a non-normal subgroup results is a set of
	//                    elements that don't form a group!
	if( !factorGroup.GenerateGroup( generatorSet, ostream ) )
		return false;

	if( wordCompressor )
	{
		if( ostream )
			*ostream << "Compressing words in factor group...\n";

		wordCompressor->Compress( factorGroup );
	}

	if( !factorGroup.unstableSet.IsEmpty() )
	{
		subGroup = new StabilizerChainGroup();

		if( ostream )
			*ostream << "Generating generators for stabilizer subgroup...\n";

		// Use Schreier's lemma here to find generators for the subgroup.
		for( uint i = 0; i < generatorSet.elementArray.size(); i++ )
		{
			const Element* generator = generatorSet.elementArray[i];

			for( uint j = 0; j < factorGroup.elementArray.size(); j++ )
			{
				const Element* cosetRepresentative = factorGroup.elementArray[j];

				std::auto_ptr< Element > product( subGroup->generatorSet.Multiply( cosetRepresentative, generator ) );

				uint offset = -1;
				if( !factorGroup.IsMember( product.get(), &offset ) )
					return false;

				const Element* cosetRepresentativeForProduct = factorGroup.elementArray[ offset ];
				std::auto_ptr< Element > invCosetRepresentativeForProduct( subGroup->generatorSet.Invert( cosetRepresentativeForProduct ) );
				
				Element* newGenerator = subGroup->generatorSet.Multiply( product.get(), invCosetRepresentativeForProduct.get() );
				subGroup->generatorSet.AddNewMember( newGenerator );
			}
		}

		if( !subGroup->Generate( factorGroup.unstableSet, wordCompressor, ostream ) )
			return false;
	}

	return true;
}

bool StabilizerChainGroup::Factor( const PermutationElement& permElement, PermutationElement& invPermElement ) const
{
	if( !subGroup )
		return true;

	if( factorGroup.IsInDivsorGroup( permElement.permutation ) )
		return subGroup->Factor( permElement, invPermElement );

	uint offset = -1;
	if( !const_cast< CosetRepresentativeSet* >( &factorGroup )->IsMember( &permElement, &offset ) )
		return false;

	const Element* cosetRepresentative = factorGroup.elementArray[ offset ];

	std::auto_ptr< Element > invCosetRepresentative( factorGroup.Invert( cosetRepresentative ) );
	
	invPermElement.MultiplyOnRight( invCosetRepresentative.get() );

	std::auto_ptr< Element > reducedElement( factorGroup.Multiply( &permElement, invCosetRepresentative.get() ) );
	
	return subGroup->Factor( *dynamic_cast< PermutationElement* >( reducedElement.get() ), invPermElement );
}

void StabilizerChainGroup::Print( std::ostream& ostream ) const
{
	ostream << "===============================================================\n";
	ostream << "Unstable set...\n";
	factorGroup.unstableSet.Print( ostream );
	ostream << "Generator set...\n";
	generatorSet.Print( ostream );
	ostream << "Factor group...\n";
	factorGroup.Print( ostream );
	if( subGroup )
		subGroup->Print( ostream );
}

// StabilizerChain.cpp