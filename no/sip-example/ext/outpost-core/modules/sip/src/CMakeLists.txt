file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
add_library(outpost_sip STATIC ${sources})
target_include_directories(outpost_sip PUBLIC ".")

if (NOT TARGET outpost_sip_parameter)
    add_library(outpost_sip_parameter INTERFACE)
    target_include_directories(outpost_sip_parameter INTERFACE "../default")
endif()

target_link_libraries(outpost_sip
    outpost_transport
    outpost_sip_parameter
    outpost_coding
)
