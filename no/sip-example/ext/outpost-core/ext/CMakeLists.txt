file(GLOB_RECURSE sources_googletest CONFIGURE_DEPENDS "./googletest-1.12.1-fused/*.cc")
add_library(googletest-local ${sources_googletest})
target_include_directories(googletest-local PUBLIC "./googletest-1.12.1-fused")
target_link_libraries(googletest-local pthread)

add_library(backward INTERFACE)
target_include_directories(backward INTERFACE "./backward-cpp")

file(GLOB_RECURSE sources_rapidcheck CONFIGURE_DEPENDS "./rapidcheck/src/*.cpp" "./rapidcheck/src/**/*.cpp")
add_library(rapidcheck STATIC ${sources_rapidcheck})

target_include_directories(rapidcheck PUBLIC
    "./rapidcheck/include"
    "./rapidcheck/extras/gmock/include"
    "./rapidcheck/extras/gtest/include"
)