cmake_minimum_required(VERSION 3.26)
set (PROJECT_NAME circular_buffer_fixed)
project(${PROJECT_NAME}
        VERSION 0.1
        DESCRIPTION "Circular Buffer Fixed"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fconcepts-diagnostics-depth=3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -D_LIBCPP_DEBUG=1")

find_package(GTest REQUIRED)
if(GTest_FOUND)
    message(STATUS "MAIN FOUND GTest, including and linking...")
    include_directories(${GTEST_INCLUDE_DIRS})
else()
    message(STATUS "NOT FOUND GTest")
endif()

add_executable(${PROJECT_NAME}
        tests.cpp
)
target_link_libraries(${PROJECT_NAME}
        GTest::GTest
        pthread
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})