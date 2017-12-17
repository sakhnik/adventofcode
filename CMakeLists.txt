cmake_minimum_required(VERSION 3.10)
enable_testing()
add_definitions(-std=c++17)

foreach(src RANGE 1 17)
    add_executable(${src} ${src}.cpp)
    target_compile_definitions(${src} PRIVATE INPUT="${CMAKE_SOURCE_DIR}/${src}.txt")
    add_test(${src} ${src})
endforeach(src)
