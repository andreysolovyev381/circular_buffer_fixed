cmake_minimum_required(VERSION 3.19)
set (PROJECT_NAME "Optimized Circular Buffer")
project(${PROJECT_NAME}
        VERSION 0.1
        DESCRIPTION "Optimized Circular Buffer, no deleter is called, using C++20"
        LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(./include)
add_subdirectory(./tests)
add_subdirectory(./benchmark)