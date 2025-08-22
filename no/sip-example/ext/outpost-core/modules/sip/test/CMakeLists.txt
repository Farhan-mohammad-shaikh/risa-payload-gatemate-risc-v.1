file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_sip
    outpost_rtos
    outpost_base_stubs
    outpost_hal_stubs
    outpost_transport_harness
    googletest-local
)

outpost_add_test(MODULE outpost_sip SOURCES ${sources} DEPENDENCIES ${deps})
