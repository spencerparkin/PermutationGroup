# CMakeListst.txt for LibPermGroup library.

set(LIB_PERM_GROUP_SOURCES
	Source/FactorGroup.cpp
	Source/FactorGroup.h
	Source/NaturalNumberSet.cpp
	Source/NaturalNumberSet.h
	Source/Permutation.cpp
	Source/Permutation.h
	Source/PermutationStream.cpp
	Source/PermutationStream.h
	Source/StabilizerChain.cpp
	Source/StabilizerChain.h
)

add_library(PermGroup STATIC ${LIB_PERM_GROUP_SOURCES})

if(UNIX)
	target_compile_options(PermGroup PRIVATE
		-fPIC
	)
endif()

target_include_directories(PermGroup PUBLIC
	Source
)

target_compile_definitions(PermGroup PRIVATE
	_CRT_SECURE_NO_WARNINGS
)
