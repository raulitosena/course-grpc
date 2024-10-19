
cmake_minimum_required(VERSION 3.28)
project(grpc-course)

set(GRPC_DIR ${CMAKE_CURRENT_LIST_DIR})

# gRPC files
add_subdirectory(00-proto)

# Synchronous samples
add_subdirectory(01-sum)
add_subdirectory(02-fibonacci)
add_subdirectory(03-average)
add_subdirectory(04-evens)

# Asynchronous samples
add_subdirectory(05-sum-async)
add_subdirectory(06-sum-callback)
add_subdirectory(07-fibonacci-callback)
add_subdirectory(08-average-callback)
add_subdirectory(09-evens-callback)

# gRPC features
add_subdirectory(20-errors)
add_subdirectory(22-interceptor)
add_subdirectory(23-multiplex)
add_subdirectory(24-cancel)
add_subdirectory(25-retry)
add_subdirectory(26-wait_for_ready)
add_subdirectory(27-deadline)
add_subdirectory(28-load_balance)
add_subdirectory(29-health)
add_subdirectory(30-keepalive)
add_subdirectory(31-compress)
add_subdirectory(32-ssl)
