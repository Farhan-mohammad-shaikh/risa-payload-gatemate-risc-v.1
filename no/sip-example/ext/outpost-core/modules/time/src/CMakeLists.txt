file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_time STATIC ${sources})
target_include_directories(outpost_time PUBLIC ".")
target_link_libraries(outpost_time outpost_base)