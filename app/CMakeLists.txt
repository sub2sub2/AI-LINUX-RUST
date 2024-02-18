cmake_minimum_required(VERSION 3.10)
project(MyApp)

message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
message(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
message(SOURCES="${SOURCES}src/main.cpp")


# Create the shared library
add_library(grpc SHARED grpc.cpp)

# Specify the installation path for the library
install(TARGETS grpc DESTINATION lib)

# Specify the installation path for the header file
install(FILES include/grpc.h DESTINATION include)


# 헤더 파일이 포함된 디렉토리 지정
include_directories(/usr/local/include)

# 라이브러리 파일이 있는 디렉토리 추가
link_directories(/usr/local/lib)
# 소스 파일 목록
set(SOURCES
    src/main.cpp
)

# 실행 파일 생성
add_executable(myapp ${SOURCES})

# 라이브러리 링크
target_link_libraries(myapp grpc)
