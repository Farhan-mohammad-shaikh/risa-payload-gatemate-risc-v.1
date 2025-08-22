file(GLOB_RECURSE sources CONFIGURE_DEPENDS "./outpost/**/*.cpp")
# no source files yet
#add_library(outpost_utils STATIC ${sources})
add_library(outpost_utils INTERFACE)
#target_include_directories(outpost_utils PUBLIC ".")
target_include_directories(outpost_utils INTERFACE ".")
# none
# target_link_libraries(outpost_utils outpost_base gsl outpost_rtos)
