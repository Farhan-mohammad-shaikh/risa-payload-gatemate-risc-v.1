file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_smpc STATIC ${sources})
target_include_directories(outpost_smpc PUBLIC ".")
target_link_libraries(outpost_smpc
    outpost_utils
    outpost_container
    outpost_rtos
)
