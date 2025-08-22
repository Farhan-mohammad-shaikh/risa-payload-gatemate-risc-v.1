file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_rtos_none STATIC ${sources})
target_include_directories(outpost_rtos_none PUBLIC ".")
target_link_libraries(outpost_rtos_none outpost_rtos outpost_time)