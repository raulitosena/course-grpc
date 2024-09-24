
project(00-proto)

set(TARGET_LIB "coursegrpc")
set(TARGET_PROTOC "protoc_compile")

set(BUILD_DIR "${GRPC_DIR}/build/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BUILD_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BUILD_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

file(GLOB SRCS_PROTO "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto")
file(GLOB SRCS_CC "${CMAKE_CURRENT_LIST_DIR}/proto/*.cc")

add_custom_target(${TARGET_PROTOC}
	COMMAND protoc --proto_path="${CMAKE_CURRENT_LIST_DIR}/proto" --cpp_out="${CMAKE_CURRENT_LIST_DIR}/proto" "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto"
	COMMAND protoc --proto_path="${CMAKE_CURRENT_LIST_DIR}/proto" --grpc_out="${CMAKE_CURRENT_LIST_DIR}/proto" --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` "${CMAKE_CURRENT_LIST_DIR}/proto/*.proto"
	COMMENT "Generating protocol buffers and gRPC services from .proto files..."
)
set_property(TARGET ${TARGET_PROTOC} PROPERTY FOLDER ${PROJECT_NAME})

if (SRCS_CC)

	add_library(${TARGET_LIB} STATIC
		"${SRCS_CC}"
	)
	set_property(TARGET ${TARGET_LIB} PROPERTY FOLDER ${PROJECT_NAME})

	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		add_dependencies(${TARGET_LIB} ${TARGET_PROTOC})
	endif()

	target_link_libraries(${TARGET_LIB} gRPC::grpc++)

endif()





