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
add_subdirectory(${API})

include_directories(${CMAKE_SOURCE_DIR}/${GRPC})
include_directories(${CMAKE_SOURCE_DIR}/${API})

# Create the main executable
add_executable(TARGET grpc_client.cpp)
# add_executable(MyExecutable src/main.cpp)

# Specify dependencies
# add_dependencies(MyExecutable ModuleA ModuleB)


# Link the modules to the main executable
# target_link_libraries(MyExecutable GRPC API)

# target_include_directories(MyExecutable PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${GRPC})
# target_include_directories(MyExecutable PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${API})
include_directories("${CMAKE_CURRENT_BINARY_DIR}"/${GRPC})

target_link_libraries(TARGET 
    # ${CMAKE_CURRENT_BINARY_DIR}/${GRPC} 
    # ${CMAKE_CURRENT_BINARY_DIR}/${API}
    hw_grpc_proto
    absl::flags
    absl::flags_parse
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF})


# include(common.cmake)

# # 헤더 파일이 포함된 디렉토리 지정
# include_directories(/usr/local/include)

# # 라이브러리 파일이 있는 디렉토리 추가
# link_directories(/usr/local/lib)
# # 소스 파일 목록
# set(SOURCES
#     src/main.cpp
# )

# # 실행 파일 생성
# add_executable(myapp ${SOURCES})

# # 라이브러리 링크
# target_link_libraries(myapp appEx)



