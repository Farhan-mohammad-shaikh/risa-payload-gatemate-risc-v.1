file(GLOB_RECURSE harness_stubs CONFIGURE_DEPENDS "./harness/**/*.cpp")
add_library(outpost_transport_harness STATIC ${harness_stubs})
target_include_directories(outpost_transport_harness PUBLIC ".")
target_link_libraries(outpost_transport_harness outpost_transport)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_transport
    outpost_transport_harness
    outpost_base_stubs
    outpost_hal_stubs
    googletest-local
)

outpost_add_test(MODULE outpost_transport SOURCES ${sources} DEPENDENCIES ${deps})
