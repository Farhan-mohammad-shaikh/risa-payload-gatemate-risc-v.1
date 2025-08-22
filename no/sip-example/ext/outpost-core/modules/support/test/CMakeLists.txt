file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_smpc_stubs
    outpost_time_stubs
    outpost_support
    outpost_base_stubs
    outpost_utils_stubs
    googletest-local
)

outpost_add_test(MODULE outpost_support SOURCES ${sources} DEPENDENCIES ${deps})
