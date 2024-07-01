set(EXECUTABLE_NAME circular_buffer_benchmark)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
            -g
            -O0
            -march=native
            -fconcepts-diagnostics-depth=3
    )
else()
    add_compile_options(
            -O3
            -march=native
    )
endif()
add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror
)

#Threads not found
IF(APPLE)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
ENDIF()

# BOOST STARTS HERE-------------------
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package (Boost REQUIRED)
if(Boost_FOUND)
    message(STATUS "MAIN: FOUND Boost, including and linking...")
    include_directories(${BOOST_ROOT})
else()
    message(STATUS "TESTS: NOT FOUND Boost")
endif()

find_package(benchmark REQUIRED)
if(benchmark_FOUND)
    message(STATUS "BENCHMARK FOUND, including and linking...")
    include_directories(${benchmark_INCLUDE_DIRS})
else()
    message(STATUS "NOT FOUND BENCHMARK")
endif()

add_executable(${EXECUTABLE_NAME}
        ./main.cpp
)
target_link_libraries(${EXECUTABLE_NAME}
        ${Boost_LIBRARIES}
        benchmark::benchmark
        pthread
)
target_compile_options(${EXECUTABLE_NAME}
        PRIVATE
        -Wno-uninitialized
)
