file(GLOB_RECURSE sources_stubs CONFIGURE_DEPENDS "./unittest/**/*.cpp")
add_library(outpost_swb_stubs STATIC ${sources_stubs})
target_include_directories(outpost_swb_stubs PUBLIC ".")
target_link_libraries(outpost_swb_stubs outpost_swb)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_base_stubs
    outpost_swb_stubs
    outpost_time_stubs
    googletest-local
)

outpost_add_test(MODULE outpost_swb SOURCES ${sources} DEPENDENCIES ${deps})
