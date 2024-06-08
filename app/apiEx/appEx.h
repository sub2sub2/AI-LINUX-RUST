#ifndef GRPC_H
#define GRPC_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <grpcpp/grpcpp.h>
#include "agent.grpc.pb.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using namespace agent;
using namespace std;

namespace AppEx {

template <typename StubType>
class AgentClient {
public:
    AgentClient(std::shared_ptr<Channel> channel)
        : stub_(StubType::NewStub(channel)) {}

    std::string inference(
        const char* id,
        const float sepal_length,
        const float sepal_width,
        const float petal_length,
        const float petal_width);

    std::string inference(const std::string& filePath);

private:
    std::unique_ptr<typename StubType::StubInterface> stub_;
};

template <typename StubType>
std::string AgentClient<StubType>::inference(
    const char* id,
    const float sepal_length,
    const float sepal_width,
    const float petal_length,
    const float petal_width) 
{
    std::cout << "check 1" << std::endl;
    IrisInferenceRequest request;
    request.set_id(id);
    request.set_sepal_length(sepal_length);
    request.set_sepal_width(sepal_width);
    request.set_petal_length(petal_length);
    request.set_petal_width(petal_width);

    IrisInferenceResponse response;
    ClientContext context;
    context.AddMetadata("authorization", "Bearer some-secret-token");

    std::cout << "check 2" << std::endl;

    Status status = stub_->Inference(&context, request, &response);
    std::cout << "check 3" << std::endl;

    if (status.ok()) {
        return response.species();
    } else {
        return "RPC failed";
    }
}

template <typename StubType>
std::string AgentClient<StubType>::inference(const std::string& filePath) 
{
    MnistInferenceRequest request;
    std::cout << filePath << std::endl;
    request.set_file_path(filePath.c_str());

    MnistInferenceResponse response;
    ClientContext context;
    context.AddMetadata("authorization", "Bearer some-secret-token");

    Status status = stub_->Inference(&context, request, &response);
    
    if (status.ok()) {
        return response.file_path();
    } else {
        return "RPC failed";
    }
}

template <typename ClientType>
class ApiEx {
public:
    ApiEx(const std::string& ipAddr, int port);

    vector<string> getAllApi(); 
    int init();

    int request(
        const char* id, 
        const float sepal_length, 
        const float sepal_width, 
        const float petal_length, 
        const float petal_width, 
        std::unique_ptr<std::string>& response);

    int request(const std::string& filePath, std::unique_ptr<std::string>& response);

private:
    std::unique_ptr<ClientType> client;
    vector<string>* apiList;
};

template <typename ClientType>
ApiEx<ClientType>::ApiEx(const std::string& ipAddr, int port) {
    std::cout << ipAddr << std::endl;
    std::cout << port << std::endl;
    std::string serverInfo = ipAddr + ":" + std::to_string(port);
    auto channel = grpc::CreateChannel(serverInfo, grpc::InsecureChannelCredentials());
    client = std::make_unique<ClientType>(channel);

    apiList = new vector<string>{"init", "request"};
}

template <typename ClientType>
vector<string> ApiEx<ClientType>::getAllApi() {
    return *apiList;
}

template <typename ClientType>
int ApiEx<ClientType>::init() {
    return 0;
}

template <typename ClientType>
int ApiEx<ClientType>::request(
    const char* id, 
    const float sepal_length, 
    const float sepal_width, 
    const float petal_length, 
    const float petal_width, 
    std::unique_ptr<std::string>& response) 
{
    try {
        response = std::make_unique<std::string>(
            client->inference(id, sepal_length, sepal_width, petal_length, petal_width)
        );
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

template <typename ClientType>
int ApiEx<ClientType>::request(
    const std::string& filePath, 
    std::unique_ptr<std::string>& response) 
{
    try {
        response = std::make_unique<std::string>(client->inference(filePath));
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

} // namespace AppEx

#endif // GRPC_H
