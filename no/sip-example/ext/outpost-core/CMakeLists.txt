cmake_minimum_required(VERSION 3.18.4)

project(outpost-core)

set(OUTPOST_OS "UNITTEST_RTOS")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# order of includes is important
include(outpost-core/UtilsUnittests) # activate assertion
include(outpost-core/Library) # use assertion
include(outpost-core/Test) # depends on target in Library

outpost_generate_test()