cmake_minimum_required(VERSION 3.10)

include(FetchContent)
FetchContent_Declare(
    ut
    GIT_REPOSITORY "https://github.com/boost-ext/ut"
    GIT_TAG "v2.0.1"
)
FetchContent_MakeAvailable(ut)

project(AdventOfCode)
find_package(Threads)
find_program(MAXIMA maxima)
if(NOT MAXIMA)
    message(STATUS "Maxima not found for computer algebra, will disable 2023-24")
    add_definitions(-DNO_MAXIMA)
endif()

enable_testing()

list(APPEND sources main.cpp)

foreach(year RANGE 2015 2023)
    foreach(src RANGE 1 25)
        string(REGEX REPLACE "^(.)$" "0\\1" zero_src ${src})
        list(APPEND sources ${year}/${zero_src}.cpp)
        add_test(NAME ${year}-${zero_src} COMMAND ${CMAKE_BUILD_DIRECTORY}/aoc "${year}-${zero_src}*")
    endforeach()
endforeach()

add_definitions(-std=c++20 -Wall -DOPENSSL_API_COMPAT=19999)

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

add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
include_directories(${ut_SOURCE_DIR}/include)
add_executable(aoc main.cpp ${sources})
target_link_libraries(aoc ${CMAKE_THREAD_LIBS_INIT} -lssl -lcrypto -lgmp -lboost_json)
target_precompile_headers(aoc PRIVATE pch.hpp)
