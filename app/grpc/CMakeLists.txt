# # Add the source file to the library
# add_library(agent_grpc OBJECT 
#   agent.pb.h
#   agent.grpc.pb.cc
#   agent.grpc.pb.h
#   agent.pb.cc)

# # Create the shared library from the object files
# add_library(agent_grpc_shared SHARED $<TARGET_OBJECTS:agent_grpc>)


# # add_library(agent_grpc 
# #   agent.pb.cc
# #   agent.pb.h
# #   agent.grpc.pb.cc
# #   agent.grpc.pb.h)

# target_link_libraries(agent_grpc 
#   protobuf::libprotobuf
#   gRPC::grpc++
#   ${_REFLECTION}
#   )

include("${CMAKE_SOURCE_DIR}/apiEx/common.cmake")



# Proto file
get_filename_component(hw_proto "../../protos/agent.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_SOURCE_DIR}/grpc/agent.pb.cc")
set(hw_proto_hdrs "${CMAKE_SOURCE_DIR}/grpc/agent.pb.h")
set(hw_grpc_srcs "${CMAKE_SOURCE_DIR}/grpc/agent.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_SOURCE_DIR}/grpc/agent.grpc.pb.h")
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_SOURCE_DIR}/grpc"
        --cpp_out "${CMAKE_SOURCE_DIR}/grpc"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")



add_library(agent_grpc_shared 
  agent.pb.h
  agent.pb.cc
  agent.grpc.pb.h
  agent.grpc.pb.cc)

# Set the -fPIC flag for all source files in the library
set_target_properties(agent_grpc_shared PROPERTIES POSITION_INDEPENDENT_CODE ON)


# Link libraries
target_link_libraries(agent_grpc_shared
  absl::flags
  absl::flags_parse
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  ${_REFLECTION}
)

