#include <string>
#include <vector>

#ifndef GRPC_H
#define GRPC_H

using namespace std;

class Object {
protected:
    void * body;
public:
};

namespace gRPC
{


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

    // XXX: should we implement with interface IApi?
    class ApiEx {
    
    public:

        // ApiEx();

        // show up all the api
        vector<string> getAllApi(); 
        
        // register app as on mca
        ApiExResponseItem init();

        // send data as a request
        ApiExResponseItem request(ApiExRequestItem data);
    
    private:
        vector<string> apiList { "init", "request" };
    };

    
}    

#endif // EXAMPLE_H

