file(GLOB_RECURSE sources_stubs CONFIGURE_DEPENDS "./unittest/**/*.cpp")

add_library(outpost_utils_harness INTERFACE)
target_include_directories(outpost_utils_harness INTERFACE ".")

add_library(outpost_utils_stubs INTERFACE)
target_include_directories(outpost_utils_stubs INTERFACE ".")

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_base_stubs
    outpost_utils_stubs
    outpost_utils_harness
    rapidcheck
    googletest-local
)

outpost_add_test(MODULE outpost_utils SOURCES ${sources} DEPENDENCIES ${deps})