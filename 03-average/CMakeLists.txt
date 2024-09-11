cmake_minimum_required(VERSION 3.28)
project(03-average)
set (SERVER_NAME server)
set (CLIENT_NAME client)

set(BUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR})

file(GLOB PROTO_FILES "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto")
file(GLOB PROTO_SRCS "${CMAKE_CURRENT_LIST_DIR}/proto/*.cc")

add_custom_target(pre_build_commands
	COMMAND protoc --proto_path=${CMAKE_CURRENT_LIST_DIR}/proto --cpp_out=${CMAKE_CURRENT_LIST_DIR}/proto ${CMAKE_CURRENT_LIST_DIR}/proto/*.proto 
	COMMAND protoc --proto_path=${CMAKE_CURRENT_LIST_DIR}/proto --grpc_out=${CMAKE_CURRENT_LIST_DIR}/proto --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ${CMAKE_CURRENT_LIST_DIR}/proto/*.proto 
	COMMENT "Generating protocol buffers and gRPC services..."
)

add_executable(${SERVER_NAME}
	src/server.cpp
	${PROTO_SRCS}
)

add_executable(${CLIENT_NAME}
	src/client.cpp
	${PROTO_SRCS}
)

# serversum dependency - protoc compilation & gRPC library
add_dependencies(${SERVER_NAME} pre_build_commands)
target_link_libraries(${SERVER_NAME} gRPC::grpc++)

# clientsum dependency - protoc compilation
add_dependencies(${CLIENT_NAME} pre_build_commands)
target_link_libraries(${CLIENT_NAME} gRPC::grpc++)

# protolib created from generated C++ protobuf code
add_library(protolib ${PROTO_FILES})
target_link_libraries(protolib gRPC::grpc++)
