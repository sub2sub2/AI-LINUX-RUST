cmake_minimum_required(VERSION 3.10)
project(MyApp)


message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
message(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
message(CMAKE_CURRENT_BINARY_DIR="${CMAKE_CURRENT_BINARY_DIR}")
message(SOURCES="${SOURCES}")

include(apiEx/common.cmake)


add_subdirectory(grpc)
add_subdirectory(apiEx)

include_directories(${CMAKE_SOURCE_DIR}/grpc)
include_directories(${CMAKE_SOURCE_DIR}/apiEx)

# add_custom_target(CompileModules DEPENDS grpc apiEx)

# Create the main executable
add_executable(MyExecutable src/main.cpp)

# add_dependencies(MyExecutable CompileModules)

## necessary 

target_link_libraries(MyExecutable
#   grpc 
  apiEx
  absl::flags
  absl::flags_parse
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  ${_REFLECTION}
)