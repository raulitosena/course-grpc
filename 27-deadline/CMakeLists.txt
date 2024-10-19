
project(27-deadline)

set (SERVER_NAME "server-27")
set (CLIENT_NAME "client-27")

set(BUILD_DIR ${GRPC_DIR}/build/${CMAKE_BUILD_TYPE}/${PROJECT_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BUILD_DIR})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${SERVER_NAME}
	src/server.cpp
)

add_executable(${CLIENT_NAME}
	src/client.cpp
)

target_include_directories(${SERVER_NAME} PRIVATE ${GRPC_DIR}/00-proto)
target_include_directories(${CLIENT_NAME} PRIVATE ${GRPC_DIR}/00-proto)

target_link_libraries(${SERVER_NAME} coursegrpc)
target_link_libraries(${CLIENT_NAME} coursegrpc)

set_property(TARGET ${SERVER_NAME} PROPERTY FOLDER ${PROJECT_NAME})
set_property(TARGET ${CLIENT_NAME} PROPERTY FOLDER ${PROJECT_NAME})