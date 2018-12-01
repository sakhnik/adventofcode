enable_testing()
add_definitions(-std=c++17 -Wall)

foreach(src RANGE 1 25)
    set(t 2016-${src})
    add_executable(${t} ${src}.cpp)
    target_compile_definitions(${t} PRIVATE INPUT="${CMAKE_CURRENT_SOURCE_DIR}/${src}.txt")
    target_link_libraries(${t} ${CMAKE_THREAD_LIBS_INIT})
    add_test(${t} ${t})
endforeach(src)

target_link_libraries(2016-5 -lssl -lcrypto)
target_link_libraries(2016-14 -lssl -lcrypto)
target_link_libraries(2016-17 -lssl -lcrypto)
