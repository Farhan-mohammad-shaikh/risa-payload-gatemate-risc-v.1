file(GLOB_RECURSE sources_stubs CONFIGURE_DEPENDS "./unittest/*.cpp")
add_library(outpost_base_stubs STATIC ${sources_stubs})
target_include_directories(outpost_base_stubs PUBLIC ".")
target_link_libraries(outpost_base_stubs gsl)

# when building the libraries for testing, outpost_base depends on
# outpost_base_stubs because it use OUTPOST_ASSERT. But in the
# src/CMakeLists.txt outpost_base_stubs can not be used since it is testing
# only -> add the dependency here
target_link_libraries(outpost_base INTERFACE outpost_base_stubs)

target_link_libraries(
    outpost_base_stubs
    outpost_base
    backward
    googletest-local
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_base_stubs
    outpost_base
    googletest-local
)

outpost_add_test(MODULE outpost_base SOURCES ${sources} DEPENDENCIES ${deps})
