// StabilizerChain.h

#pragma once

#include "Permutation.h"
#include "NaturalNumberSet.h"
#include <vector>
#include <iostream>
#include <string>

typedef std::vector< uint > UintArray;

class StabilizerChain
{
public:

	StabilizerChain( void );
	virtual ~StabilizerChain( void );

	bool Generate( const PermutationSet& generatorSet, const UintArray& baseArray );
	void Print( std::ostream& ostream ) const;

	class OrbitNode;
	class Group;

	typedef std::vector< OrbitNode* > OrbitNodeArray;

	class OrbitNode
	{
	public:

		OrbitNode( const Permutation* cosetRepresentative );
		~OrbitNode( void );

		bool Grow( Group* group, const PermutationSet& generatorSet, const PermutationSet& singletonSet, bool fresh, OrbitNodeArray& newOrbitArray );

		const Permutation* cosetRepresentative;
		OrbitNodeArray adjacentNodeArray;
	};

	class Group
	{
	public:

		Group( StabilizerChain* stabChain, uint stabilizerOffset );
		virtual ~Group( void );

		bool Extend( const Permutation& generator );
		bool IsMember( const Permutation& permutation ) const;
		bool FactorInverse( const Permutation& permutation, Permutation& invPermutation ) const;
		PermutationSet::iterator FindCoset( const Permutation& permutation );
		uint GetSubgroupStabilizerPoint( void ) const;
		void Print( std::ostream& ostream ) const;
		bool StabilizesPoint( uint point ) const;

		NaturalNumberSet orbitSet;
		OrbitNode* rootNode;
		uint stabilizerOffset;
		PermutationSet generatorSet;
		PermutationSet transversalSet;
		Group* subGroup;
		StabilizerChain* stabChain;
	};

	Group* group;
	UintArray baseArray;
	std::ostream* logStream;
	NaturalNumberSet freeOffsetSet;
};

// StabilizerChain.h