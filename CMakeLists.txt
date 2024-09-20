
cmake_minimum_required(VERSION 3.28)
project(grpc-course)

set(GRPC_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(00-proto)
add_subdirectory(01-sum)
add_subdirectory(02-fibonacci)
add_subdirectory(03-average)
add_subdirectory(04-evens)
add_subdirectory(05-sum-async)
add_subdirectory(06-sum-async-thread)
add_subdirectory(07-sum-callback-default)
add_subdirectory(08-sum-callback)
add_subdirectory(09-fibonacci-callback)
add_subdirectory(10-average-callback)