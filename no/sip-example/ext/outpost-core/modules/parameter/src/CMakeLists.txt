file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_parameter STATIC ${sources})
target_include_directories(outpost_parameter PUBLIC ".")
target_link_libraries(outpost_parameter
    outpost_time
)
