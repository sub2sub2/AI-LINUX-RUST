// #pragma once
#include <string>
#include <vector>
#include "appEx.h"

using namespace std;

namespace AppEx
{

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
