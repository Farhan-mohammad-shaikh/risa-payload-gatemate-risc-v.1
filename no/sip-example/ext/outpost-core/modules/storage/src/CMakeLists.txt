file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_storage STATIC ${sources})
target_include_directories(outpost_storage PUBLIC ".")
target_link_libraries(outpost_storage
    outpost_container
)
