cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(AI_Model)

find_package(PkgConfig REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(OpenCV REQUIRED)

pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(GLIB REQUIRED glib-2.0)

add_executable(MnistModel MnistModel.cpp Model.cpp)


include_directories(${DBUS_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
include_directories("/usr/include/dbus-1.0")
include_directories("/usr/include/jsoncpp")
include_directories("/usr/local/include/opencv4")


target_link_libraries(MnistModel onnxruntime ${DBUS_LIBRARIES} ${GLIB_LIBRARIES} ${OpenCV_LIBS} JsonCpp::JsonCpp)
