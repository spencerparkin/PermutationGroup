// StabilizerChain.h

#pragma once

#include "ElementSet.h"

//------------------------------------------------------------------------------------------
//                                   StabilizerChainGroup
//------------------------------------------------------------------------------------------

#if 0

class StabilizerChainGroup
{
public:

	StabilizerChainGroup( void );
	~StabilizerChainGroup( void );

	bool Generate( const NaturalNumberSet& domainSet, std::ostream* ostream = nullptr );

	// The given inverse element should be initialized to the identity element before this call.
	bool Factor( const Element& element, Element& invElement ) const;

	ElementCollection generatorSet;
	CosetCollection factorGroup;

	StabilizerChainGroup* subGroup;
};

#endif

// StabilizerChain.h