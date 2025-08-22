file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_base_stubs
    outpost_comm
    outpost_hal_stubs
    outpost_swb_stubs
    googletest-local
)

outpost_add_test(MODULE outpost_comm SOURCES ${sources} DEPENDENCIES ${deps})
