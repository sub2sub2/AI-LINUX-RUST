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

#include "agent.grpc.pb.h"
#include "appEx.h"

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

using namespace AppEx;

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint specified by
  // the argument "--target=" which is the only expected argument.
  std::string target_str = absl::GetFlag(FLAGS_target);
  // We indicate that the channel isn't authenticated (use of
  // InsecureChannelCredentials()).
  AgentClient<IrisInference> appGrpc(
      grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  
  // std::string user("world");
  std::string reply = appGrpc.Inference("id", 1., 2., 3., 4.);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
