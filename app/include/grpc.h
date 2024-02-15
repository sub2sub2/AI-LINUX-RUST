#include <string>


#ifndef GRPC_H
#define GRPC_H

namespace gRPC
{
    enum eAPI {
        API_BEGIN,
        API_INIT = API_BEGIN,
        API_REGISTER,
        API_REQUEST,
        API_END
    };

    std::string enumToString(eAPI api);

}    

#endif // EXAMPLE_H

