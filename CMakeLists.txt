cmake_minimum_required(VERSION 3.10)
project(JVC)

set(CMAKE_CXX_STANDARD 17)

include_directories(BEFORE include)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(libs)
