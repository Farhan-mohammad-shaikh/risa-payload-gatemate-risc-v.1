add_library(outpost_smpc_stubs INTERFACE)
target_include_directories(outpost_smpc_stubs INTERFACE ".")
target_link_libraries(outpost_smpc_stubs INTERFACE outpost_smpc)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")

set(deps
    outpost_smpc_stubs
    outpost_base_stubs
    googletest-local
)

outpost_add_test(MODULE outpost_smpc SOURCES ${sources} DEPENDENCIES ${deps})