#include <iostream>
#include <stdio.h>
#include <string>
// #include <agent.grpc.pb.h>
#include <vector>
#include <appEx.h>

using namespace std;
// using namespace gRPC;


int main() {
    AppEx::ApiEx api("192.178.0.1:8080"); 
    
    bool flag = true;
    while (flag){
        vector<string> apiList = api.getAllApi(); 

        // print Options
        int i = 1;
        std::cout << "\n===============================" << endl;
        std::cout << "Enter an option (1, 2, 3, etc.):"  << std::endl;
        for(auto a : apiList)
        {
            std::cout << i++ << ". " << a << std::endl;
        }
        std::cout << "q. " << "quit" << std::endl;
        std::cout << "===============================" << endl;


        // input option
        std::cout << "Please write your option: ";
        char option;    
        cin >> option;

        switch (option) {
        case 'q': // quit
            std::cout << ">>> Option q selected.\n"; 
            flag = false;
            break;

        case '1': // init
            std::cout << ">>> Option 'init' selected.\n";
            break;

        case '2': // request
            std::cout << ">>> Option 'request' selected.\n";
            break;
        // Add more cases for other options as needed

        default:
            std::cout << ">>> Invalid option. Please choose a valid option.\n";
            break;

    }

        
    }
    
    cout << "hi" << endl;

    return 0;
}