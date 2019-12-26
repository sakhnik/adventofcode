cmake_minimum_required(VERSION 3.10)
project(AdventOfCode)
find_package(Threads)

list(APPEND sources main.cpp)

foreach(src RANGE 1 20)
    string(REGEX REPLACE "^(.)$" "0\\1" zero_src ${src})
    list(APPEND sources 2015/${zero_src}.cpp)
endforeach()

foreach(year RANGE 2016 2019)
    foreach(src RANGE 1 25)
        string(REGEX REPLACE "^(.)$" "0\\1" zero_src ${src})
        list(APPEND sources ${year}/${zero_src}.cpp)
    endforeach()
endforeach()

add_definitions(-std=c++17 -Wall)

foreach(src ${sources})
    string(REPLACE ".cpp" ".txt" input ${src})
    get_filename_component(input_txt ${input} REALPATH BASE_DIR ${CMAKE_SOURCE_DIR})
    string(REPLACE ".cpp" "" test_name ${src})
    set_property(
        SOURCE ${src}
        APPEND
        PROPERTY COMPILE_DEFINITIONS
        INPUT="${input_txt}"
        TEST_NAME="${test_name}"
    )
endforeach()

add_executable(aoc ${sources})
target_link_libraries(aoc ${CMAKE_THREAD_LIBS_INIT} -lssl -lcrypto -lgmp)
