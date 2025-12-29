# CMakeLists.txt for test program.

set(TEST_SOURCES
    Test.cpp
)

add_executable(TestProgram ${TEST_SOURCES})

target_link_libraries(TestProgram PRIVATE PermGroup)