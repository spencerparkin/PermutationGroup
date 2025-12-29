# CMakeLists.txt for permutation group project.

cmake_minimum_required(VERSION 3.27)

project(PermutationGroup)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(LibPermGroup)

option(ENABLE_TEST_PROGRAM "Build test program for the library." ON)
option(ENABLE_PY_MODULE "Build Python bindings module for the library." ON)

if(ENABLE_TEST_PROGRAM)
	add_subdirectory(Test)
endif()

if(ENABLE_PY_MODULE)
	add_subdirectory(PyPermGroup)
endif()
