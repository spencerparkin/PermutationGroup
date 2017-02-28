// StabilizerChain.cpp

#include "StabilizerChain.h"

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

bool StabilizerChainGroup::Generate( const NaturalNumberSet& domainSet )
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

	if( !factorGroup.GenerateGroup() )
		return false;

	if( !factorGroup.unstableSet.IsEmpty() )
	{
		subGroup = new StabilizerChainGroup();

		// Use Streier's lemma here to find generators for the subgroup.
		for( ElementSet::const_iterator iter = generatorSet.elementSet.cbegin(); iter != generatorSet.elementSet.cend(); iter++ )
		{
			const Element& generator = *iter;

			for( ElementSet::const_iterator cosetIter = factorGroup.elementSet.cbegin(); iter != factorGroup.elementSet.cend(); iter++ )
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

// StabilizerChain.cpp