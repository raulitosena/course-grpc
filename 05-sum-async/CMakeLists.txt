
project(05-sum-async)

set (SERVER_NAME "server-05")
set (CLIENT_NAME "client-05")
set (CLIENT_NAME2 "client-thread-05")

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

add_executable(${CLIENT_NAME2}
	src/client-thread.cpp
)

target_include_directories(${SERVER_NAME} PRIVATE ${GRPC_DIR}/00-proto)
target_include_directories(${CLIENT_NAME} PRIVATE ${GRPC_DIR}/00-proto)
target_include_directories(${CLIENT_NAME2} PRIVATE ${GRPC_DIR}/00-proto)

target_link_libraries(${SERVER_NAME} coursegrpc)
target_link_libraries(${CLIENT_NAME} coursegrpc)
target_link_libraries(${CLIENT_NAME2} coursegrpc)

set_property(TARGET ${SERVER_NAME} PROPERTY FOLDER ${PROJECT_NAME})
set_property(TARGET ${CLIENT_NAME} PROPERTY FOLDER ${PROJECT_NAME})
set_property(TARGET ${CLIENT_NAME2} PROPERTY FOLDER ${PROJECT_NAME})