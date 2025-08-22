file(GLOB_RECURSE harness_stubs CONFIGURE_DEPENDS "./harness/**/*.cpp")
add_library(outpost_hal_harness STATIC ${harness_stubs})
target_include_directories(outpost_hal_harness PUBLIC ".")

target_link_libraries(outpost_hal_harness
    outpost_utils_harness
    outpost_hal
)

file(GLOB_RECURSE sources_stubs CONFIGURE_DEPENDS "./unittest/**/*.cpp")
add_library(outpost_hal_stubs STATIC ${sources_stubs})
target_include_directories(outpost_hal_stubs PUBLIC ".")
target_link_libraries(outpost_hal_stubs outpost_hal  outpost_time outpost_time_stubs)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_base_stubs
    outpost_hal_stubs
    outpost_hal_harness
    outpost_utils_harness
    googletest-local
)

outpost_add_test(MODULE outpost_hal SOURCES ${sources} DEPENDENCIES ${deps})
