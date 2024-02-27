
#ifndef GRPC_H
#define GRPC_H


#include <iostream>
#include <memory>
#include <string>
#include <vector>



#include "appEx.h"
#include "agent.grpc.pb.h"
#include <grpcpp/grpcpp.h>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using agent::IrisInference;
using agent::IrisInferenceResponse;
using agent::IrisInferenceRequest;
using namespace std;

class Object {
protected:
    void * body;
public:
};

namespace AppEx
{



class AgentClient 
{
public:
    AgentClient(std::shared_ptr<Channel> channel);

    // Assembles the client's payload, sends it and presents the response back
    // from the server.
    std::string inference(
        const char* id,
        const float sepal_length,
        const float sepal_width,
        const float petal_length,
        const float petal_width);

private:    
    std::unique_ptr<IrisInference::Stub> stub_;
};

// XXX: should we implement with interface IApi?
class ApiEx  {

public:
    ApiEx(string ipAddr);

    // show up all the api
    // XXX: Need to be capsulated 
    vector<string> getAllApi(); 
    
    // register app as on mca
    // XXX: Need to be capsulated.
    int init();

    // send data as a request
    // XXX: Should response be wrapped by wrapper? and for what?
    int request(
        const char* id, 
        const float sepal_length, 
        const float sepal_width, 
        const float petal_length, 
        const float petal_width, 
        std::string response);

private:
    AgentClient* client;
    vector<string> *apiList;
};
  
}    

#endif // EXAMPLE_H

