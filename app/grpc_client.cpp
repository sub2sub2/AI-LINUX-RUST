/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 * 
 */

#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include <grpcpp/grpcpp.h>

#include "m_grpc/agent.grpc.pb.h"
#include "api/appEx.h"

// change
ABSL_FLAG(std::string, target, "localhost:8080", "Server address");

/**
service Greeter {
  // Sends a greeting
  rpc SayHello (HelloRequest) returns (HelloReply) {}

  rpc SayHelloStreamReply (HelloRequest) returns (stream HelloReply) {}

  rpc SayHelloBidiStream (stream HelloRequest) returns (stream HelloReply) {}
} 
*/


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using agent::IrisInference;
using agent::IrisInferenceResponse;
using agent::IrisInferenceRequest;

class AgentClient {
 public:
  AgentClient(std::shared_ptr<Channel> channel)
      : stub_(IrisInference::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string Inference(
    const char* id,
    const float sepal_length,
    const float sepal_width,
    const float petal_length,
    const float petal_width
    ) {
    // Data we are sending to the server.
    IrisInferenceRequest request;
    request.set_id(id);
    request.set_sepal_length(sepal_length);
    request.set_sepal_width(sepal_width);
    request.set_petal_length(petal_length);
    request.set_petal_width(petal_width);

    // Container for the data we expect from the server.
    IrisInferenceResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->Inference(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.species();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<IrisInference::Stub> stub_;
};

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  std::string target_str = absl::GetFlag(FLAGS_target);
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  AgentClient appGrpc(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  
  // std::string user("world");
  std::string reply = appGrpc.Inference("id", 1., 2., 3., 4.);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
