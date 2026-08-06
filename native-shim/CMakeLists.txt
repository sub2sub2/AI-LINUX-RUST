cmake_minimum_required(VERSION 3.10)
project(contextengine_tester_shim CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)

# GBS(packaging/contextengine-tester-shim.spec)가 %cmake로 이 파일을 그대로 구동한다.
# TIDL 생성 라이브러리 연결 전까지는 실제 빌드가 되지 않는다 (README.md 참고).
#
# 필요 패키지 (디바이스/에뮬레이터 빌드 시):
#   - capi-appfw-app-control (app_control API)
#   - glib-2.0 (메인 루프)
#   - sqlite3 (DB 직접 쿼리 — db_reader.cpp)
#   - nlohmann_json (헤더 온리)
#   - contextengine TIDL 생성 라이브러리 (내부 코드, 이 저장소에는 없음 — TODO로 표시)

find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(APP_CONTROL REQUIRED capi-appfw-app-control)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)

add_executable(contextengine_tester_shim
    src/main.cpp
    src/app_control_client.cpp
    src/tidl_subscribe_client.cpp
    src/bridge_socket_server.cpp
    src/db_reader.cpp
)

target_include_directories(contextengine_tester_shim PRIVATE
    include
    ${GLIB_INCLUDE_DIRS}
    ${APP_CONTROL_INCLUDE_DIRS}
    ${SQLITE3_INCLUDE_DIRS}
)

target_link_libraries(contextengine_tester_shim PRIVATE
    nlohmann_json::nlohmann_json
    ${GLIB_LIBRARIES}
    ${APP_CONTROL_LIBRARIES}
    ${SQLITE3_LIBRARIES}
    # TODO: contextengine TIDL 생성 라이브러리 링크 추가
)

install(TARGETS contextengine_tester_shim DESTINATION ${CMAKE_INSTALL_BINDIR})
# tizen-manifest.xml / db_config.json은 packaging/*.spec의 %install 단계에서 직접 배치한다.
