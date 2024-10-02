
cmake_minimum_required(VERSION 3.28)
project(grpc-course)

set(GRPC_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(00-proto)
# Synchronous samples
add_subdirectory(01-sum)
add_subdirectory(02-fibonacci)
add_subdirectory(03-average)
add_subdirectory(04-evens)

# Asynchronous samples (single-thread)
add_subdirectory(05-sum-async)
add_subdirectory(06-sum-callback)
add_subdirectory(07-fibonacci-callback)
add_subdirectory(08-average-callback)
add_subdirectory(09-evens-callback)

# Asynchronous samples (multithread)
# To be done

# gRPC features
add_subdirectory(20-errors)
add_subdirectory(21-deadline)
add_subdirectory(22-interceptor)
add_subdirectory(23-multiplex)
add_subdirectory(24-cancel)
add_subdirectory(25-retry)
