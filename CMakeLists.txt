cmake_minimum_required(VERSION 3.10)
project(JVC)

set(CMAKE_CXX_STANDARD 17)

set(JVC_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
include_directories(BEFORE "${JVC_INCLUDE_DIR}")

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(libs)
