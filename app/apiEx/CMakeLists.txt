message("${CMAKE_CURRENT_BINARY_DIR}")



add_library(apiEx SHARED 
  appEx.h
  FileServer.h
)

target_link_libraries(apiEx PRIVATE
  agent_grpc_shared
  gRPC::grpc++
  )

# target_include_directories(apiEx PRIVATE
#   "/home/sseo/Proj/AI-LINUX-RUST/grpc/include/grpcpp"
#  )

target_include_directories(apiEx PRIVATE "${CMAKE_SOURCE_DIR}/grpc")

target_include_directories(apiEx PRIVATE
  ${GRPCPP_INCLUDE_DIR}
)