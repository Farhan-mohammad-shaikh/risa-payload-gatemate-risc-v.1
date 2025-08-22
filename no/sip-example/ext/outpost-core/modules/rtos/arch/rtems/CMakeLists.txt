file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_rtos_rtems STATIC ${sources})
target_include_directories(outpost_rtos_rtems PUBLIC ".")
target_link_libraries(outpost_rtos_rtems outpost_rtos outpost_time outpost_rtems)