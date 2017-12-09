cmake_minimum_required(VERSION 3.10)                                                                               
add_definitions(-std=c++17)

foreach(src RANGE 1 9)
	add_executable(${src} ${src}.cpp)
endforeach(src)
