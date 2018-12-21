cmake_minimum_required(VERSION 3.10)
find_package(Threads)

list(APPEND sources main.cpp)

foreach(src RANGE 1 2)
    list(APPEND sources 2015/${src}.cpp)
endforeach()

foreach(year RANGE 2016 2017)
    foreach(src RANGE 1 25)
        list(APPEND sources ${year}/${src}.cpp)
    endforeach()
endforeach()

foreach(src RANGE 01 21)
    string(REGEX REPLACE "^(.)$" "0\\1" zero_src ${src})
    list(APPEND sources 2018/${zero_src}.cpp)
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
target_link_libraries(aoc ${CMAKE_THREAD_LIBS_INIT} -lssl -lcrypto)
