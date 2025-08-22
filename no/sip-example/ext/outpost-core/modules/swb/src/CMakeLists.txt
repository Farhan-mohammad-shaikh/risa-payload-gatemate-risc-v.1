add_library(outpost_swb INTERFACE)
target_include_directories(outpost_swb INTERFACE .)
target_link_libraries(outpost_swb INTERFACE outpost_support)
