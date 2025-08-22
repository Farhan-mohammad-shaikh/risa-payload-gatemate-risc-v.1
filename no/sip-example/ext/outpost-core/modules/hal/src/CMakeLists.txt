file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_hal STATIC ${sources})
target_include_directories(outpost_hal PUBLIC ".")

if (NOT TARGET outpost_hal_parameter)
    add_library(outpost_hal_parameter INTERFACE)
    target_include_directories(outpost_hal_parameter INTERFACE "../default")
endif()

target_link_libraries(outpost_hal
    outpost_swb
    outpost_base
    outpost_hal_parameter
)
