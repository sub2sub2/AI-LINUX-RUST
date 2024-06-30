#include <iostream>
#include <stdio.h>
#include <string>
// #include <agent.grpc.pb.h>
#include <vector>
#include <appEx.h>

using namespace std;
using namespace AppEx;


std::string getStringInput()
{
    std::string input;
    std::getline(std::cin, input);

    return input;
}

std::vector<float> getInput()
{
    std::cout << "Enter four float numbers separated by commas >>> " <<std::endl;

    // flush buffer    
    cin.ignore(1000, '\n');

    std::string input;
    std::getline(std::cin, input);

    std::vector<float> numbers;
    if (!input.empty()) {
        std::istringstream stream(input);

        float num;
        char comma;

        while (stream >> num) {
            numbers.push_back(num);
            stream >> comma; // Read the comma
        }
    }
    else
    {
        // default values 
        numbers.push_back(1.0);   
        numbers.push_back(2.0);   
        numbers.push_back(3.0);   
        numbers.push_back(4.0);   
    }

    
    

    // Check if exactly four numbers were entered
    if (numbers.size() == 4) {
        // Now you can use numbers[0], numbers[1], numbers[2], and numbers[3]
        std::cout << "You entered: " << numbers[0] << ", " << numbers[1] << ", "
                  << numbers[2] << ", " << numbers[3] << std::endl;
    } else {
        std::cout << "Invalid input. Please enter exactly four float numbers separated by commas." << std::endl;
    }

    return numbers;
}


int main() {

    int id = 0; 
    bool flag = true;
    while (flag){

        // print Options
        int i = 1;
        std::cout << "\n===============================" << endl;
        std::cout << "Enter an option (1, 2, 3, etc.):"  << std::endl;

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
            std::cout << ">>> Option 'init' selected. (NOT IMPLEMENTED YET)\n";
            break;

        case '2': {
            // request
            ApiEx<AgentClient<IrisInference>> api("localhost", 8080); 

            std::cout << ">>> Option 'request' selected." << std::endl;

            // float sepal_length, sepal_width, petal_length, petal_width;
            std::vector<float> i = getInput();
            
            std::unique_ptr<std::string> result;
            auto response = api.request(
                const_cast<char*>(std::to_string(id).c_str()), // sorry, it could be fixed
                i[0], i[1], i[2], i[3],
                result
            );

            if (response == 0) {
                std::cout << "Response: " << *result.get() << std::endl;            
            } else 
            {
                std::cout << "gRPC not working" << std::endl;
            }
            
            break;
        }
        // Add more cases for other options as needed
        case '3': {
            // request
            ApiEx<AgentClient<MnistInference>> api("localhost", 8081); 

            std::cout << ">>> Option 'request path' selected." << std::endl;

            auto filePath = getStringInput();
            if (filePath.empty())  
                filePath = "testtesttest";
            std::cout << filePath << std::endl;
            std::unique_ptr<std::string> result;
            auto response = api.request(
                filePath,
                result
            );

            if (response == 0) {
                std::cout << "Response: " << *result.get() << std::endl;            
            } else 
            {
                std::cout << "gRPC not working" << std::endl;
            }
            
            break;
        }
    case '4': {
            // request
            ApiEx<AgentClient<FileInference>> api("localhost", 8082); 

            std::cout << ">>> Option 'request path' selected." << std::endl;

            auto filePath = getStringInput();
            if (filePath.empty())  
                filePath = "testtesttest";
            std::cout << filePath << std::endl;
            auto response = api.request(
                "/home/hyunsub/workspace/hyunsub/AI-LINUX-RUST/largefile.txt",
                false
            );

            if (response == 0) {
                std::cout << "Response: " << response << std::endl;            
            } else 
            {
                std::cout << "gRPC not working" << std::endl;
            }
            
            break;
        }

        default:
            std::cout << ">>> Invalid option. Please choose a valid option.\n";
            break;

        }
        cin.ignore(1000, '\n');

        
    }
    
    cout << "hi" << endl;

    return 0;
}