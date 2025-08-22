file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_rtos_freertos STATIC ${sources})
target_include_directories(outpost_rtos_freertos PUBLIC ".")
target_link_libraries(outpost_rtos_freertos outpost_rtos outpost_time outpost_freertos)