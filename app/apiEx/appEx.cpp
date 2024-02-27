// #pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "appEx.h"

#include "agent.grpc.pb.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

// Declare and define the ABSL_FLAG
ABSL_FLAG(std::string, target, "localhost:8080", "Server address");


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using agent::IrisInference;
using agent::IrisInferenceResponse;
using agent::IrisInferenceRequest;


namespace AppEx
{


ApiEx::ApiEx(string ipAddr) {
    
    std::cout << ipAddr << std::endl;
    // Access the value of the ABSL_FLAG using the Flag object
    std::string target_str = absl::GetFlag(FLAGS_target);

    // Now use target_str to create AgentClient and assign to the pointer
    client = new AgentClient(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));

    apiList = new vector<std::string> {
        "init", "request"
    };

}


vector<string> ApiEx::getAllApi()
{
    return *apiList;
}

int ApiEx::request(const char* id, const float sepal_length, const float sepal_width, const float petal_length, const float petal_width, std::string response)
{

    std::cout << "In the request" << std::endl;
    try
    {
        response = client->inference(id, sepal_length, sepal_width, petal_length, petal_width);    
    }
    catch (std::exception& e){

        std::cout << "In the request, catch()" << std::endl;

        std::cout << e.what() << std::endl;
        return 1; 
    }
    return 0;

}

#pragma region AgentClient

AgentClient::AgentClient(std::shared_ptr<Channel> channel)
    : stub_(IrisInference::NewStub(channel)) {}

std::string AgentClient::inference (
    const char* id,
    const float sepal_length,
    const float sepal_width,
    const float petal_length,
    const float petal_width ) 
{
    std::cout << "In inference" << std::endl;

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
#pragma endregion

}
