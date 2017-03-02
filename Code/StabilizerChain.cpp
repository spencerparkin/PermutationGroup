// StabilizerChain.cpp

#include "StabilizerChain.h"

#if 0

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

bool StabilizerChainGroup::Generate( const NaturalNumberSet& domainSet, std::ostream* ostream /*= nullptr*/ )
{
	factorGroup.unstableSet.Copy( domainSet );
	NaturalNumberSet::UintSet::iterator iter = factorGroup.unstableSet.set.begin();
	factorGroup.unstableSet.RemoveMember( *iter );

	for( ElementSet::const_iterator iter = generatorSet.elementSet.cbegin(); iter != generatorSet.elementSet.cend(); iter++ )
	{
		const Element& generator = *iter;
		Element element( generator );
		factorGroup.AddElement( element );
	}

	if( ostream )
	{
		*ostream << "Generating factor group for unstable set...\n";
		factorGroup.unstableSet.Print( *ostream );
		*ostream << "\n";
	}

	if( !factorGroup.GenerateGroup( ostream ) )
		return false;

	if( !factorGroup.unstableSet.IsEmpty() )
	{
		subGroup = new StabilizerChainGroup();

		if( ostream )
			*ostream << "Generating generators for stabilizer subgroup...\n";

		// Use Streier's lemma here to find generators for the subgroup.
		for( ElementSet::const_iterator iter = generatorSet.elementSet.cbegin(); iter != generatorSet.elementSet.cend(); iter++ )
		{
			const Element& generator = *iter;

			for( ElementSet::const_iterator cosetIter = factorGroup.elementSet.cbegin(); cosetIter != factorGroup.elementSet.cend(); cosetIter++ )
			{
				const Element& cosetRepresentative = *cosetIter;

				Element product;
				product.Multiply( generator, cosetRepresentative );

				const Element* cosetRepresentativeForProduct = factorGroup.FindCosetRepresentative( product );
				if( !cosetRepresentativeForProduct )
					return false;

				Element invCosetRepresentativeForProduct;
				invCosetRepresentativeForProduct.SetInverse( *cosetRepresentativeForProduct );

				Element newGenerator;
				newGenerator.Multiply( product, invCosetRepresentativeForProduct );

				subGroup->generatorSet.AddElement( newGenerator );
			}
		}

		if( !subGroup->Generate( factorGroup.unstableSet ) )
			return false;
	}

	return true;
}

bool StabilizerChainGroup::Factor( const Element& element, Element& invElement ) const
{
	if( !subGroup )
		return true;

	if( factorGroup.IsInQuotientGroup( element.permutation ) )
		return subGroup->Factor( element, invElement );

	const Element* cosetRepresentative = factorGroup.FindCosetRepresentative( element );
	if( !cosetRepresentative )
		return false;

	Element invCosetRepresentative;
	invCosetRepresentative.SetInverse( *cosetRepresentative );

	invElement.MultiplyOnRight( invCosetRepresentative );

	Element reducedElement;
	reducedElement.Multiply( element, invCosetRepresentative );

	return subGroup->Factor( reducedElement, invElement );
}

#endif

// StabilizerChain.cpp