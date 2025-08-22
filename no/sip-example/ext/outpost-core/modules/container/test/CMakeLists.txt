file(GLOB_RECURSE sources_stubs CONFIGURE_DEPENDS "./unittest/*.cpp")
add_library(outpost_container_stubs STATIC ${sources_stubs})
target_include_directories(outpost_container_stubs PUBLIC ".")
#target_link_libraries(outpost_container_stubs gsl)

target_link_libraries(
    outpost_container_stubs
    outpost_base
    backward
    googletest-local
)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_base_stubs
    outpost_container_stubs
    googletest-local
)

outpost_add_test(MODULE outpost_container SOURCES ${sources} DEPENDENCIES ${deps})
