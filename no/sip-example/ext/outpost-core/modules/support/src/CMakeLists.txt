file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_support STATIC ${sources})
target_include_directories(outpost_support PUBLIC ".")

if (NOT TARGET outpost_support_parameter)
    add_library(outpost_support_parameter INTERFACE)
    target_include_directories(outpost_support_parameter INTERFACE "../default")
endif()

target_link_libraries(outpost_support
    outpost_smpc
    outpost_support_parameter
)
