file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_transport STATIC ${sources})
target_include_directories(outpost_transport PUBLIC ".")

target_link_libraries(outpost_transport
    outpost_hal
    outpost_coding
)
