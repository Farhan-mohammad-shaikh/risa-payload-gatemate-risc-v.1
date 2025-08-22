file(GLOB_RECURSE sources_stubs CONFIGURE_DEPENDS "./unittest/*.cpp")
# none yet
#add_library(outpost_coding_stubs STATIC ${sources_stubs})
#target_include_directories(outpost_coding_stubs PUBLIC ".")
add_library(outpost_coding_stubs INTERFACE)
target_include_directories(outpost_coding_stubs INTERFACE ".")

# dependencies of the stubs
#target_link_libraries(outpost_coding_stubs gsl)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/**/*.cpp")
set(deps
    outpost_base
    outpost_coding
    outpost_coding_stubs
    googletest-local
    rapidcheck
)

outpost_add_test(MODULE outpost_coding SOURCES ${sources} DEPENDENCIES ${deps})
