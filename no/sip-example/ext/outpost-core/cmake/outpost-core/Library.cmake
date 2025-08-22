# The most external libraries are used only during testing on posix.
# But gsl is also used in the flight software. Therefor it is added
# here instead of in ./ext/CMakeLists.txt
add_library(gsl INTERFACE)
target_include_directories(gsl INTERFACE "${CMAKE_CURRENT_LIST_DIR}/../../ext/gsl/include")

# To provide a custom port create a library with the name `outpost_rtos_port`
# If `outpost_rtos_port` is defined no outpost-core rtos port will be used.
# This allows a project to use a cusom port without touching the outpost-core
# repository.
if (NOT TARGET outpost_rtos_port)
    set(ARCH "${CMAKE_CURRENT_LIST_DIR}/../../modules/rtos/arch")
    if (OUTPOST_OS STREQUAL "POSIX")
        add_subdirectory("${ARCH}/posix" outpost-core/modules/rtos/arch/posix)
        add_library(outpost_rtos_port ALIAS outpost_rtos_posix)
    elseif (OUTPOST_OS STREQUAL "NONE")
        add_subdirectory("${ARCH}/none" outpost-core/modules/rtos/arch/none)
        add_library(outpost_rtos_port ALIAS outpost_rtos_none)
    elseif(OUTPOST_OS STREQUAL "RTEMS")
        add_subdirectory("${ARCH}/rtems" outpost-core/modules/rtos/arch/rtems)
        add_library(outpost_rtos_port ALIAS outpost_rtos_rtems)
    elseif(OUTPOST_OS STREQUAL "FREERTOS")
        add_subdirectory("${ARCH}/freertos" outpost-core/modules/rtos/arch/freertos)
        add_library(outpost_rtos_port ALIAS outpost_rtos_freertos)
    elseif(OUTPOST_OS STREQUAL "UNITTEST_RTOS")
        add_subdirectory("${ARCH}/unittest_rtos" outpost-core/modules/rtos/arch/unittest_rtos)
        add_library(outpost_rtos_port ALIAS outpost_rtos_unittest)
    else()
        message(FATAL_ERROR "Please select a RTOS port (NONE, POSIX, RTEMS, FREERTOS, UNITTEST_RTOS) or provide a custom implementation")
    endif()
endif()

file(GLOB modules LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/../../modules/*/src")

foreach (module ${modules})
    get_filename_component(path_to_module "${module}/.." ABSOLUTE)
    get_filename_component(module_name "${path_to_module}" NAME)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../modules/${module_name}/src" outpost-core/modules/${module_name})
endforeach()