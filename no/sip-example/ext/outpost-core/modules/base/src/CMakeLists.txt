add_library(outpost_base INTERFACE)
target_include_directories(outpost_base INTERFACE ".")
target_link_libraries(outpost_base INTERFACE gsl)
