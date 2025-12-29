# CMakeLists.txt for PyPermGroup module.

set(PY_PERM_GROUP_SOURCES
    Source/PyPerm.cpp
    Source/PyPerm.h
    Source/PyPermGroup.cpp
    Source/PyPermGroup.h
    Source/PyStabChain.cpp
    Source/PyStabChain.h
)

add_library(PyPermGroup MODULE ${PY_PERM_GROUP_SOURCES})

target_link_libraries(PyPermGroup PRIVATE
    PermGroup
)

set_target_properties(PyPermGroup PROPERTIES PREFIX "")
set_target_properties(PyPermGroup PROPERTIES SUFFIX ".so")

target_include_directories(PyPermGroup PRIVATE
    "/usr/include/python3.13"
)