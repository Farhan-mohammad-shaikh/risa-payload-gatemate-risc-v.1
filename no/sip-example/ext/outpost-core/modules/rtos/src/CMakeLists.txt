# rtos
file(GLOB_RECURSE rtos_sources CONFIGURE_DEPENDS "./outpost/rtos/**.cpp")
add_library(outpost_rtos STATIC ${rtos_sources})
target_include_directories(outpost_rtos PUBLIC ".")

if (NOT TARGET outpost_rtos_parameter)
    add_library(outpost_rtos_parameter INTERFACE)
    target_include_directories(outpost_rtos_parameter INTERFACE "../default")
endif()

target_link_libraries(outpost_rtos
    outpost_rtos_port
    outpost_rtos_parameter
    outpost_time
)

# rtos_utils
file(GLOB_RECURSE rtos_utils_sources CONFIGURE_DEPENDS "./outpost/rtos_utils/**.cpp")
add_library(outpost_rtos_utils STATIC ${rtos_utils_sources})
target_include_directories(outpost_rtos_utils PUBLIC ".")

target_link_libraries(outpost_rtos_utils
    outpost_rtos
    outpost_time
)
