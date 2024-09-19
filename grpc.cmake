
set (SERVER_NAME "server-${PROJECT_NAME}")
set (CLIENT_NAME "client-${PROJECT_NAME}")
set (PROTOC_COMPILATION "protocbuild-${PROJECT_NAME}")
set (PROTOLIB_NAME "protolib-${PROJECT_NAME}")

set(BUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/build/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

include_directories(${CMAKE_CURRENT_LIST_DIR})

file(GLOB PROTO_FILES "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto")
file(GLOB PROTO_SRCS "${CMAKE_CURRENT_LIST_DIR}/proto/*.cc")

add_custom_target(${PROTOC_COMPILATION}
	COMMAND protoc --proto_path=${CMAKE_CURRENT_LIST_DIR}/proto --cpp_out=${CMAKE_CURRENT_LIST_DIR}/proto ${CMAKE_CURRENT_LIST_DIR}/proto/*.proto 
	COMMAND protoc --proto_path=${CMAKE_CURRENT_LIST_DIR}/proto --grpc_out=${CMAKE_CURRENT_LIST_DIR}/proto --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ${CMAKE_CURRENT_LIST_DIR}/proto/*.proto 
	COMMENT "Generating protocol buffers and gRPC services..."
)
set_property(TARGET ${PROTOC_COMPILATION} PROPERTY FOLDER ${PROJECT_NAME})

add_executable(${SERVER_NAME}
	${PROTO_SRCS}
	src/server.cpp
)
set_property(TARGET ${SERVER_NAME} PROPERTY FOLDER ${PROJECT_NAME})

add_executable(${CLIENT_NAME}
	${PROTO_SRCS}
	src/client.cpp
)
set_property(TARGET ${CLIENT_NAME} PROPERTY FOLDER ${PROJECT_NAME})

# protolib created from generated C++ protobuf code
add_library(${PROTOLIB_NAME}
	${PROTO_FILES}
)
set_property(TARGET ${PROTOLIB_NAME} PROPERTY FOLDER ${PROJECT_NAME})
target_link_libraries(${PROTOLIB_NAME} gRPC::grpc++)

# serversum dependency - protoc compilation & gRPC library
add_dependencies(${SERVER_NAME} ${PROTOC_COMPILATION})
target_link_libraries(${SERVER_NAME} gRPC::grpc++)

# clientsum dependency - protoc compilation
add_dependencies(${CLIENT_NAME} ${PROTOC_COMPILATION})
target_link_libraries(${CLIENT_NAME} gRPC::grpc++)
