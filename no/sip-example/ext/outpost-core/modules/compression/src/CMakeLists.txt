file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_compression STATIC ${sources})
target_include_directories(outpost_compression PUBLIC ".")
target_link_libraries(
    outpost_compression
    outpost_support
    outpost_rtos_utils
)
