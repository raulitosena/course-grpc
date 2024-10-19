
project(33-reflection)

set (SERVER_NAME "server-33")

set(BUILD_DIR ${GRPC_DIR}/build/${CMAKE_BUILD_TYPE}/${PROJECT_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

add_executable(${SERVER_NAME}
	src/FibonacciServiceImpl.hpp
	src/FibonacciSlowServiceImpl.hpp
	src/server.cpp
)

target_include_directories(${SERVER_NAME} PRIVATE ${GRPC_DIR}/00-proto)

target_link_libraries(${SERVER_NAME} coursegrpc gRPC::grpc++_reflection)

set_property(TARGET ${SERVER_NAME} PROPERTY FOLDER ${PROJECT_NAME})