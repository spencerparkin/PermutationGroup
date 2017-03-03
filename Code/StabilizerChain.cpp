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

	if( ostream )
		*ostream << "Compressing word in generator set...\n";

	if( wordCompressor )
		wordCompressor->Compress( generatorSet );

	if( ostream )
	{
		*ostream << "Generators for subgroup in stabilizer chain...\n";

		for( ElementList::const_iterator iter = generatorSet.elementList.cbegin(); iter != generatorSet.elementList.cend(); iter++ )
		{
			const Element* generator = *iter;
			if( ostream )
				generator->Print( *ostream );
		}

		*ostream << "Generating factor group for unstable set...\n";
		factorGroup.unstableSet.Print( *ostream );
		*ostream << "\n";
	}

	if( !factorGroup.GenerateGroup( generatorSet, ostream ) )
		return false;

	if( !factorGroup.unstableSet.IsEmpty() )
	{
		subGroup = new StabilizerChainGroup();

		if( ostream )
			*ostream << "Generating generators for stabilizer subgroup...\n";

		// Use Schreier's lemma here to find generators for the subgroup.
		for( ElementList::const_iterator iter = generatorSet.elementList.cbegin(); iter != generatorSet.elementList.cend(); iter++ )
		{
			const Element* generator = *iter;

			for( ElementList::const_iterator cosetIter = factorGroup.elementList.cbegin(); cosetIter != factorGroup.elementList.cend(); cosetIter++ )
			{
				const Element* cosetRepresentative = *cosetIter;

				std::auto_ptr< Element > product( subGroup->generatorSet.Multiply( cosetRepresentative, generator ) );

				ElementList::iterator memberIter;
				if( !factorGroup.IsMember( product.get(), &memberIter ) )
					return false;

				const Element* cosetRepresentativeForProduct = *memberIter;
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

	ElementList::iterator memberIter;
	if( !const_cast< CosetRepresentativeSet* >( &factorGroup )->IsMember( &permElement, &memberIter ) )
		return false;

	const Element* cosetRepresentative = *memberIter;

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