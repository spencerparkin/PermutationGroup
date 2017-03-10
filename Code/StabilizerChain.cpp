// StabilizerChain.cpp

#include "StabilizerChain.h"
#include <memory>

//------------------------------------------------------------------------------------------
//                                   StabilizerChainGroup
//------------------------------------------------------------------------------------------

StabilizerChainGroup::StabilizerChainGroup( void )
{
	generatorSet.elementsHaveUniqueRepresentation = true;
	transversalSet.elementsHaveUniqueRepresentation = false;

	subGroup = nullptr;
}

StabilizerChainGroup::~StabilizerChainGroup( void )
{
	delete subGroup;
}

bool StabilizerChainGroup::Generate( uint newStabilizer, uint maxStabilizer, WordCompressor* wordCompressor /*= nullptr*/, std::ostream* ostream /*= nullptr*/ )
{
	transversalSet.stableSet.RemoveAllMembers();
	for( uint i = 0; i <= newStabilizer; i++ )
		transversalSet.stableSet.AddMember(i);

	if( wordCompressor )
	{
		if( ostream )
			*ostream << "Compressing words in generator set...\n";

		wordCompressor->Compress( generatorSet );
	}

	if( ostream )
	{
		*ostream << "Generators for subgroup in stabilizer chain...\n";
		generatorSet.Print( *ostream );
		*ostream << "Generating transversal for stable set...\n";
		transversalSet.stableSet.Print( *ostream );
		*ostream << "\n";
	}

	if( !transversalSet.GenerateGroup( generatorSet, ostream ) )
		return false;

	if( wordCompressor )
	{
		if( ostream )
			*ostream << "Compressing words in transversal...\n";

		wordCompressor->Compress( transversalSet );
	}

	if( newStabilizer < maxStabilizer )
	{
		subGroup = new StabilizerChainGroup();

		if( ostream )
			*ostream << "Generating generators for stabilizer subgroup...\n";

		// Use Schreier's lemma here to find generators for the subgroup.
		for( uint i = 0; i < generatorSet.elementArray.size(); i++ )
		{
			const Element* generator = generatorSet.elementArray[i];

			for( uint j = 0; j < transversalSet.elementArray.size(); j++ )
			{
				const Element* cosetRepresentative = transversalSet.elementArray[j];

				std::auto_ptr< Element > product( subGroup->generatorSet.Multiply( cosetRepresentative, generator ) );

				uint offset = -1;
				if( !transversalSet.IsMember( product.get(), &offset ) )
					return false;

				const Element* cosetRepresentativeForProduct = transversalSet.elementArray[ offset ];
				std::auto_ptr< Element > invCosetRepresentativeForProduct( subGroup->generatorSet.Invert( cosetRepresentativeForProduct ) );
				
				Element* newGenerator = subGroup->generatorSet.Multiply( product.get(), invCosetRepresentativeForProduct.get() );
				subGroup->generatorSet.AddNewMember( newGenerator );
			}
		}

		if( !subGroup->Generate( newStabilizer + 1, maxStabilizer, wordCompressor, ostream ) )
			return false;
	}

	return true;
}

bool StabilizerChainGroup::Factor( const PermutationElement& permElement, PermutationElement& invPermElement ) const
{
	if( !subGroup )
		return true;

	if( transversalSet.IsInDivsorGroup( permElement.permutation ) )
		return subGroup->Factor( permElement, invPermElement );

	uint offset = -1;
	if( !const_cast< StabilizerCosetRepresentativeSet* >( &transversalSet )->IsMember( &permElement, &offset ) )
		return false;

	const Element* cosetRepresentative = transversalSet.elementArray[ offset ];

	std::auto_ptr< Element > invCosetRepresentative( transversalSet.Invert( cosetRepresentative ) );
	
	invPermElement.MultiplyOnRight( invCosetRepresentative.get() );

	std::auto_ptr< Element > reducedElement( transversalSet.Multiply( &permElement, invCosetRepresentative.get() ) );
	
	return subGroup->Factor( *dynamic_cast< PermutationElement* >( reducedElement.get() ), invPermElement );
}

void StabilizerChainGroup::Print( std::ostream& ostream ) const
{
	ostream << "===============================================================\n";
	ostream << "Stable set...\n";
	transversalSet.stableSet.Print( ostream );
	ostream << "Generator set...\n";
	generatorSet.Print( ostream );
	ostream << "Transversal...\n";
	transversalSet.Print( ostream );
	if( subGroup )
		subGroup->Print( ostream );
}

// StabilizerChain.cpp