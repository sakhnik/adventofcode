cmake_minimum_required(VERSION 3.10)

include(FetchContent)
FetchContent_Declare(
    ut
    GIT_REPOSITORY "https://github.com/boost-ext/ut"
    GIT_TAG "v2.0.1"
)
FetchContent_MakeAvailable(ut)

project(AdventOfCode)
set(CXX_STANDARD 20)
find_package(Threads)
find_program(MAXIMA maxima)
if(NOT MAXIMA)
    message(STATUS "Maxima not found for computer algebra, will disable 2023-24")
    add_definitions(-DNO_MAXIMA)
endif()

enable_testing()

add_definitions(-DOPENSSL_API_COMPAT=19999)
add_compile_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
add_link_options("$<$<CONFIG:DEBUG>:-fsanitize=address>")
include_directories(${ut_SOURCE_DIR}/include)

macro(add_year year day_first day_last)
    set(aoc "aoc-${year}")
    add_executable(${aoc} main.cpp)
    target_link_libraries(${aoc} ${CMAKE_THREAD_LIBS_INIT} -lssl -lcrypto -lgmp -lboost_json -lfmt)
    target_precompile_headers(${aoc} PRIVATE pch.hpp)
    foreach(src RANGE ${day_first} ${day_last})
        string(REGEX REPLACE "^(.)$" "0\\1" zero_src ${src})
        set(src "${year}/${zero_src}.cpp")
        target_sources(${aoc} PRIVATE ${src})

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

        add_test(NAME ${year}-${zero_src} COMMAND ${CMAKE_BUILD_DIRECTORY}/${aoc} "${zero_src}*")
    endforeach()
endmacro()

foreach(year RANGE 2015 2023)
    add_year(${year} 1 25)
endforeach()

add_year("2024" 1 22)
