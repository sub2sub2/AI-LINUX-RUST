#include <string>
#include <vector>

#ifndef GRPC_H
#define GRPC_H


class Object {
protected:
    void * body;
public:
};

namespace gRPC
{

    // XXX: should we implement with interface IApi?
    class ApiEx {
    
    public:
        vector<string> getAllApi(); 
        
        ApiExResponseItem init();

        ApiExResponseItem request(ApiExRequestItem data);
    
    private:
        vector<string> apiList { "init", "request" };
    };

    // XXX: should we implement with Interface IRequestItem?  
    class ApiExRequestItem {
    
    public:
        vector<float> input; 
    };

    class ApiExResponseItem {

    public:
        int result = 0; 
        Object body;
    };
}    

#endif // EXAMPLE_H

