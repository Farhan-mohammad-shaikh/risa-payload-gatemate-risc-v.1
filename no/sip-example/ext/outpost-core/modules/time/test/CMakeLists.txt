file(GLOB_RECURSE sources_harness CONFIGURE_DEPENDS "./harness/**/*.cpp")
add_library(outpost_time_harness STATIC ${sources_harness})
target_include_directories(outpost_time_harness PUBLIC ".")
target_link_libraries(outpost_time_harness outpost_time)

file(GLOB_RECURSE sources_stubs CONFIGURE_DEPENDS "./unittest/**/*.cpp")
add_library(outpost_time_stubs STATIC ${sources_stubs})
target_include_directories(outpost_time_stubs PUBLIC ".")
target_link_libraries(outpost_time_stubs outpost_time)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_time_stubs
    outpost_time_harness
    googletest-local
    outpost_base_stubs
)

outpost_add_test(MODULE outpost_time SOURCES ${sources} DEPENDENCIES ${deps})

