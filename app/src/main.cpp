#include <iostream>
#include <stdio.h>
#include <string>
#include <grpc.h>

using namespace std;

// gRPC::eAPI getCommand ()
// {

//     // Get a character input from the user
//     std::cout << "Enter a character (Q to quit): " << endl;        
//     for (gRPC::eAPI i = gRPC::API_BEGIN; i < gRPC::API_END; i = gRPC::eAPI(i + 1))
//     {
//         // std::cout << i << ". " << (string)gRPC::eAPI << std::endl; 
//         std::cout << i << ". " << gRPC::enumToString(i) << std::endl; 
//     }

//     string cInput;
//     std::cin >> cInput;

//     int userInput = stoi(cInput.substr(0));
//     switch((gRPC::eAPI)userInput){

//         case gRPC::API_INIT:
//             return gRPC::API_INIT;
//             // cout << enumToString(API_INIT) << endl;
//     }
//     return (gRPC::eAPI)userInput;
// }


int main() {
    // auto command = getCommand();

    // while (true){
    //     switch((gRPC::eAPI)command){
    //         case gRPC::API_INIT:

    //     }
        
    // }
    
    cout << "hi" << endl;

    return 0;
}