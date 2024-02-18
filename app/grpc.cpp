// #pragma once
#include <string>
#include <vector>
#include "grpc.h"
// #include <api/grpc.h>

using namespace std;

namespace gRPC
{

    // enum eAPI {
    //     API_BEGIN,
    //     API_INIT = API_BEGIN,
    //     API_REGISTER,
    //     API_REQUEST,
    //     API_END
    // };


    // std::string get(eAPI api) 
    // {
    //     switch (api) {
    //         case API_INIT:
    //             return "API_INIT";
    //         case API_REGISTER:
    //             return "API_REGISTER";
    //         case API_REQUEST:
    //             return "API_REQUEST";
    //         // Add more cases for other commands as needed
    //         default:
    //             return "UNKNOWN_COMMAND";
    //     }
    // }

    vector<string> ApiEx::getAllApi()
    {
        return ApiEx::apiList;
    }

    ApiExResponseItem ApiEx::init()
    {
        
        return ApiExResponseItem();
    }

    ApiExResponseItem ApiEx::request(ApiExRequestItem data)
    {
        return ApiExResponseItem();
    }

}
