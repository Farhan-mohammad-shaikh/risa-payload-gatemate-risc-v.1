# note: a implementation of the unittest RTOS must be a object library,
# otherwise the link order of the static libraries can cause problems
# since there is no explicit dependencies between outpost_rtos and the
# implementation. Therefore a module that depends on outpost_rtos does
# not necessary depend on the outpost_rtos_unittest_x. This can lead to
# undefined references. When using object libraries the order does not
# matter.

file(GLOB_RECURSE sources_posix CONFIGURE_DEPENDS "./src/posix/outpost/**/*.cpp" "../common/posix/outpost/**/*.cpp")
add_library(outpost_rtos_unittest_posix OBJECT ${sources_posix})
target_include_directories(outpost_rtos_unittest_posix PUBLIC "." "../common/posix")
target_link_libraries(outpost_rtos_unittest_posix outpost_time pthread rt)

add_library(outpost_rtos_unittest INTERFACE)
target_include_directories(outpost_rtos_unittest INTERFACE "./inc")
target_link_libraries(outpost_rtos_unittest INTERFACE outpost_rtos)