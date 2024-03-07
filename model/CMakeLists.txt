cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(dcgan)

find_package(Torch REQUIRED)

add_executable(dcgan dcgan.cpp)
add_executable(net_onnx net_onnx.cpp)

target_link_libraries(dcgan "${TORCH_LIBRARIES}")
target_link_libraries(net_onnx onnxruntime)
set_property(TARGET dcgan PROPERTY CXX_STANDARD 17)