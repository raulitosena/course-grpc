cmake_minimum_required(VERSION 3.28)
project(serversum)
set (SERVICE_NAME serversum)

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
    COMMENT "Generating code from .proto files..."
)

add_executable(${SERVICE_NAME}
	src/main.cpp
	${PROTO_SRCS}
)

# serversum dependency - protoc compilation
add_dependencies(${SERVICE_NAME} pre_build_commands)
# serversum dependency - gRPC library
target_link_libraries(${SERVICE_NAME} gRPC::grpc++)
# protolib created from generated C++ protobuf code
add_library(protolib ${PROTO_FILES})
target_link_libraries(protolib gRPC::grpc++)
