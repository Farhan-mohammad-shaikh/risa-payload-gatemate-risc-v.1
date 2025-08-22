file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_compression
    outpost_time_stubs
    outpost_base_stubs
    googletest-local
)

outpost_add_test(MODULE outpost_compression SOURCES ${sources} DEPENDENCIES ${deps})