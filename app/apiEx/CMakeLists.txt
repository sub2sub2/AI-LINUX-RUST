message("${CMAKE_CURRENT_BINARY_DIR}")



add_library(apiEx SHARED 
  appEx.cpp
  appEx.h
)

target_link_libraries(apiEx PRIVATE
  agent_grpc_shared
  gRPC::grpc++
  )

# target_include_directories(apiEx PRIVATE
#   "/home/sseo/Proj/AI-LINUX-RUST/grpc/include/grpcpp"
#  )

target_include_directories(apiEx PRIVATE "${CMAKE_SOURCE_DIR}/grpc")

find_path(GRPCPP_INCLUDE_DIR grpcpp/grpcpp.h PATHS "/home/sseo/Proj/AI-LINUX-RUST/grpc/include" NO_DEFAULT_PATH)
if(NOT GRPCPP_INCLUDE_DIR)
  message(FATAL_ERROR "grpc++ headers not found. Set GRPCPP_INCLUDE_DIR to the directory containing grpcpp/grpcpp.h")
endif()

target_include_directories(apiEx PRIVATE
  ${GRPCPP_INCLUDE_DIR}
)