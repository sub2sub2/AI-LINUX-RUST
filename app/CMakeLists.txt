cmake_minimum_required(VERSION 3.10)
project(MyApp)

message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
message(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
message(SOURCES="${SOURCES}src/main.cpp")

# 헤더 파일이 포함된 디렉토리 지정
include_directories(${CMAKE_SOURCE_DIR}/include)

# 라이브러리 파일이 있는 디렉토리 추가
link_directories(${CMAKE_SOURCE_DIR}/lib)
# 소스 파일 목록
set(SOURCES
    src/main.cpp
)

# 실행 파일 생성
add_executable(myapp ${SOURCES})

# 라이브러리 링크
target_link_libraries(myapp grpc)
