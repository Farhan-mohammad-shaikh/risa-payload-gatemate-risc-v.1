file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_comm STATIC ${sources})
target_include_directories(outpost_comm PUBLIC ".")
target_link_libraries(outpost_comm
    outpost_base
    outpost_hal
    outpost_coding
)