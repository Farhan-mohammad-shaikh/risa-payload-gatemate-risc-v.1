file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_coding STATIC ${sources})
target_include_directories(outpost_coding PUBLIC ".")
target_link_libraries(outpost_coding
    outpost_base
)
