add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../ext" outpost-core/ext)

file(GLOB modules LIST_DIRECTORIES true CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/../../modules/*/test")

foreach (module ${modules})
    get_filename_component(path_to_module "${module}/.." ABSOLUTE)
    get_filename_component(module_name "${path_to_module}" NAME)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../../modules/${module_name}/test" outpost-core/modules/${module_name}/test)
endforeach()