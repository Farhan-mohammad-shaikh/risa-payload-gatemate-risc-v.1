file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp" "../common/posix/outpost/**/*.cpp")
add_library(outpost_rtos_posix STATIC ${sources})
target_include_directories(outpost_rtos_posix PUBLIC "." "../common/posix")
target_link_libraries(outpost_rtos_posix outpost_rtos outpost_time pthread rt)