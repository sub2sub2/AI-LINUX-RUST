#include <iostream>
#include <dbus/dbus.h>
#include <onnxruntime_cxx_api.h>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <jsoncpp/json/json.h>
#include <cstdlib>

const std::string interfaceName = "org.mca.Model1";
const std::string serviceName = "org.mca.Model1";
const std::string objectPath = "/org/mca/Model1";

Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");

// Configure the session options
Ort::SessionOptions session_options;
// session_options.SetIntraOpNumThreads(1);

// Load the model from the ONNX file
Ort::Session session(env, "/home/AI-LINUX-RUST/model/weights/iris.onnx", session_options);

// Get the input and output names
Ort::AllocatorWithDefaultOptions allocators;
std::vector<const char*> input_names = {"input"};
Ort::AllocatedStringPtr pstrOutputName = session.GetOutputNameAllocated(0, allocators.operator OrtAllocator * ());
auto output_names = pstrOutputName.get(); // get output name

// Allocate memory for the input and output tensors
std::vector<int64_t> input_shape = {1, 4};
size_t input_size = 1 * 4;
float* input_tensor = reinterpret_cast<float*>(allocators.Alloc(sizeof(float) * input_size));
std::vector<int64_t> output_shape = {1, 1};
size_t output_size = 1 * 1;
float* output_tensor = reinterpret_cast<float*>(allocators.Alloc(sizeof(float) * output_size));


// D-Bus 메시지 수신을 처리하기 위한 콜백 함수
DBusHandlerResult handleDBusMessage(DBusConnection* connection, DBusMessage* message, void* userdata) {
    // 인터페이스, 메서드 및 인자 확인
    const char* interface = dbus_message_get_interface(message);
    const char* method = dbus_message_get_member(message);
    
    if (strcmp(interface, interfaceName.c_str()) == 0 && strcmp(method, "Predict") == 0) {
        // MyMethod가 호출되면 실행할 코드
        std::cout << "Received D-Bus method call: " << method << std::endl;
        
        DBusMessageIter iter;
        dbus_message_iter_init(message, &iter);
        char* jsonStr;
        dbus_message_iter_get_basic(&iter, &jsonStr);
        
        // JSON 파싱
        Json::Value root;
        Json::Reader reader;
        bool parsingSuccessful = reader.parse(jsonStr, root);
        if (parsingSuccessful) {
            // 각 키의 값을 출력
            std::cout << "col 1: " << root["col1"].asString() << std::endl;
            std::cout << "col 2: " << root["col2"].asString() << std::endl;
            std::cout << "col 3: " << root["col3"].asString() << std::endl;
            std::cout << "col 4: " << root["col4"].asString() << std::endl;
        } else {
            std::cerr << "Failed to parse JSON input." << std::endl;
        }

        std::vector<float> input_data = {{std::stof(root["col1"].asString()),std::stof(root["col2"].asString()),
                                            std::stof(root["col3"].asString()),std::stof(root["col4"].asString())}};

        memcpy(input_tensor, &input_data, sizeof(float) * input_size);

        // Wrap the input and output tensors in Ort::Value objects
        Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        Ort::Value input_value = Ort::Value::CreateTensor(memory_info, input_tensor, input_size, input_shape.data(), input_shape.size());
        Ort::Value output_value = Ort::Value::CreateTensor(memory_info, output_tensor, output_size, output_shape.data(), output_shape.size());

        // Run the model and get the output
        auto output_tensors = session.Run(Ort::RunOptions{nullptr}, input_names.data(), &input_value, 1, &output_names, 1);

        // Print the output
        std::cout << "Output: " << std::endl;
        std::string result;
        for (const auto& output_tensor : output_tensors) {
            auto tensor_data = static_cast<const float*>(output_tensor.GetTensorRawData());
            size_t tensor_size = output_tensor.GetTensorTypeAndShapeInfo().GetElementCount();
            for (size_t i = 0; i < tensor_size; ++i) {
                std::cout << "test" << tensor_data[i] << std::endl;
                result = std::to_string(tensor_data[i]);
            }
        }
        const char* response = result.c_str();
        DBusMessage* reply = dbus_message_new_method_return(message);
        dbus_message_append_args(reply, DBUS_TYPE_STRING, &response, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, reply, NULL);
        dbus_message_unref(reply);             
    }
    
    return DBUS_HANDLER_RESULT_HANDLED;
}

// 타이머 클래스
class Timer {
public:
    Timer() : running(false) {}
    
    // 타이머 시작
    void start(std::chrono::seconds duration) {
        running = true;
        std::this_thread::sleep_for(duration);
        running = false;
    }
    
    // 타이머가 실행 중인지 확인
    bool isRunning() const {
        return running;
    }
    
private:
    bool running;
};

int main() {
    DBusError error;
    dbus_error_init(&error);
    
    // D-Bus 커넥션 생성
    DBusConnection* connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
    if (connection == NULL) {
        std::cerr << "Failed to get D-Bus connection: " << error.message << std::endl;
        dbus_error_free(&error);
        return 1;
    }
    
    // 타이머 생성 및 시작
    Timer timer;
    std::thread timerThread([&]() {
        timer.start(std::chrono::minutes(1)); // 1분간 실행
        // 1분 후에는 프로그램 종료
        if (!timer.isRunning()) {
            std::cout << "No D-Bus request received for 1 minute. Exiting program." << std::endl;
            dbus_connection_unref(connection);
            exit(0);
        }
    });
    
   dbus_bus_request_name(connection, interfaceName.c_str(), DBUS_NAME_FLAG_REPLACE_EXISTING, &error);
    if (dbus_error_is_set(&error)) {
        std::cerr << "Failed to register D-Bus service: " << error.message << std::endl;
        dbus_error_free(&error);
    }

    // D-Bus 메시지 수신을 처리할 콜백 함수 등록
    dbus_bus_add_match(connection, "type='method_call',interface='org.mca.Model1'", &error);
    dbus_connection_add_filter(connection, handleDBusMessage, NULL, NULL);
    dbus_connection_flush(connection);

    // 메시지 루프 시작
    while (true) {
        dbus_connection_read_write_dispatch(connection, -1);
    }
    
    // 정리 작업
    dbus_connection_remove_filter(connection, handleDBusMessage, NULL);
    dbus_connection_unref(connection);
    
    // 타이머 스레드 종료
    timerThread.join();
    
    return 0;
}
