# note: a implementation of the unittest RTOS must be a object library,
# otherwise the link order of the static libraries can cause problems
# since there is no explicit dependencies between outpost_rtos and the
# implementation. Therefore a module that depends on outpost_rtos does
# not necessary depend on the outpost_rtos_unittest_x. This can lead to
# undefined references. When using object libraries the order does not
# matter.

if(OUTPOST_OS STREQUAL "UNITTEST_RTOS")
    file(GLOB_RECURSE sources_stubs CONFIGURE_DEPENDS "./unittest/**/*.cpp" "../arch/common/posix/outpost/**/*.cpp")
    add_library(outpost_rtos_unittest_mock OBJECT ${sources_stubs})
    target_include_directories(outpost_rtos_unittest_mock PUBLIC "." "../arch/common/posix")
    target_link_libraries(outpost_rtos_unittest_mock outpost_rtos rt)

    file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/rtos/**.cpp")

    set(deps
        outpost_rtos_unittest_mock
        googletest-local
    )

    outpost_add_test(MODULE outpost_rtos SOURCES ${sources} DEPENDENCIES ${deps})
endif()

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "main.cpp" "./outpost/rtos_utils/**.cpp")

set(deps
    outpost_rtos
    outpost_rtos_utils
    googletest-local
)

outpost_add_test(MODULE outpost_rtos_utils SOURCES ${sources} DEPENDENCIES ${deps})