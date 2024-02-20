cmake_minimum_required(VERSION 3.10)
project(MyApp)

message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
message(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
message(CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
message(SOURCES="${SOURCES}src/main.cpp")


include(m_grpc/common.cmake)

set(GRPC "m_grpc")
set(API "api")

add_subdirectory(${GRPC})
# add_subdirectory(${API})

# CMAKE_CURRENT_BINARY_DIR=AI-LINUX-RUST/app/build
include_directories("${CMAKE_CURRENT_BINARY_DIR}"/${GRPC})
# include_directories(${CMAKE_SOURCE_DIR}/${GRPC})

# idk why this is not necessary
# CMAKE_SOURCE_DIR=AI-LINUX-RUST/app
# include_directories(${CMAKE_SOURCE_DIR}/${API})

# Create the main executable
add_executable(TARGET grpc_client.cpp)


# Link the modules to the main executable
# target_link_libraries(MyExecutable GRPC API)

# target_include_directories(MyExecutable PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${GRPC})
# target_include_directories(MyExecutable PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${API})

## necessary 
target_link_libraries(TARGET 
    # ${CMAKE_CURRENT_BINARY_DIR}/${GRPC} 
    # ${CMAKE_CURRENT_BINARY_DIR}/${API}
    hw_grpc_proto
    absl::flags
    absl::flags_parse
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})
