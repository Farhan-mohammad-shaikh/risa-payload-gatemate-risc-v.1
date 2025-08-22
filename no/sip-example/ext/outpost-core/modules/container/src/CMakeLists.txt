file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_container STATIC ${sources})
target_include_directories(outpost_container PUBLIC ".")
target_link_libraries(outpost_container
    outpost_utils
    outpost_traits
    outpost_rtos    # mutex
)
