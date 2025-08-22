file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_parameter
    outpost_base_stubs
    googletest-local
)

outpost_add_test(MODULE outpost_parameter SOURCES ${sources} DEPENDENCIES ${deps})