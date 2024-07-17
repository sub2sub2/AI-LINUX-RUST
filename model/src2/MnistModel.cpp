#include <iostream>
#include <dbus/dbus.h>
#include <onnxruntime_cxx_api.h>
#include <chrono>
#include <thread>
#include <string>
#include <vector>
#include <jsoncpp/json/json.h>
#include <cstdlib>
#include <syslog.h>
#include <opencv2/opencv.hpp>
#include <random>
#include <ctime>
#include <json/json.h>


#include "Timer.h"
#include "Model.h"

Json::Value readJsonFromFile(const std::string& filename) {
    std::ifstream file(filename);
    Json::Value jsonData;
    if (file.is_open()) {
        file >> jsonData;
        file.close();
    } else {
        std::cerr << "Unable to open file " << filename << std::endl;
    }
    return jsonData;
}

using namespace AIModel::Model;
using namespace AIModel::Utils;

const std::string interfaceName = "org.mca.Model2";
const std::string serviceName = "org.mca.Model2";
const std::string objectPath = "/org/mca/Model2";
const std::string modelPath = "/home/hyunsub/workspace/hyunsub/AI-LINUX-RUST/model/weights/mnist.onnx";

std::unique_ptr<ModelInstance> mModel = std::make_unique<ModelInstance>(modelPath);
Timer timer;

std::string generateRandomFilename(const std::string& prefix, const std::string& extension)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    int len = 5; // 파일 이름 길이

    std::string filename = prefix;

    // 랜덤 시드 설정
    std::mt19937 gen(static_cast<unsigned long>(time(nullptr)));
    std::uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);

    for (int i = 0; i < len; ++i) {
        filename += alphanum[dis(gen)];
    }

    filename += extension;
    return filename;
}

// D-Bus 메시지 수신을 처리하기 위한 콜백 함수
DBusHandlerResult handleDBusMessage(DBusConnection* connection, DBusMessage* message, void* userdata) {
    // 인터페이스, 메서드 및 인자 확인
    const char* interface = dbus_message_get_interface(message);
    const char* method = dbus_message_get_member(message);
    timer.reset(1);
    if (strcmp(interface, interfaceName.c_str()) == 0 && strcmp(method, "Predict") == 0) {
        // MyMethod가 호출되면 실행할 코드
        std::cout << "Received D-Bus method call: " << method << std::endl;

        DBusMessageIter iter;
        if (!dbus_message_iter_init(message, &iter)) {
            throw std::runtime_error("Message has no arguments");
        }

        if (DBUS_TYPE_STRING != dbus_message_iter_get_arg_type(&iter)) {
            throw std::runtime_error("Argument is not string");
        }

        const char* str;
        dbus_message_iter_get_basic(&iter, &str);
        auto filePath = std::string(str);
        std::cout << "File path: " << filePath << std::endl;

        Json::Value jsonData = readJsonFromFile(filePath);
        size_t batch_size = jsonData.size();
        int64_t input_size = batch_size * 1 * 28 * 28;

        std::vector<int64_t> output_shape = {batch_size, 10};
        std::vector<int64_t> input_shape = {batch_size, 1, 28, 28};
        mModel->set_input_size(input_shape);
        mModel->set_output_size(output_shape);


        std::vector<float> input_tensor_values(batch_size * 28 * 28);

        size_t i = 0;
        for (const auto& key : jsonData.getMemberNames()) {
            const Json::Value& imageValue = jsonData[key]["data"];
            std::vector<uint8_t> imageData(imageValue.size());
            for (Json::ArrayIndex j = 0; j < imageValue.size(); ++j) {
                imageData[j] = imageValue[j].asUInt();
            }

            cv::Mat img(cv::Size(28, 28), CV_8UC1, imageData.data());
            if (img.empty()) {
                std::cerr << "Failed to load image from JSON data: " << key << std::endl;
                return DBUS_HANDLER_RESULT_HANDLED;
            }

            img.convertTo(img, CV_32F, 1.0 / 255.0);
            img = (img - 0.5f) / 0.5f;

            std::memcpy(input_tensor_values.data() + i * 28 * 28, img.data, 28 * 28 * sizeof(float));
            ++i;
        }

        auto filename = generateRandomFilename("/tmp/.", "").c_str();
        auto response = mModel->infernce(input_tensor_values);

        std::ofstream outfile(filename);
        if (outfile.is_open()) {
            outfile << response;
            outfile.close();
            std::cout << "Content saved to file: " << filename << std::endl;
        } else {
            std::cerr << "Unable to open file: " << filename << std::endl;
        }

        DBusMessage* reply = dbus_message_new_method_return(message);
        dbus_message_append_args(reply, DBUS_TYPE_STRING, &filename, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, reply, NULL);
        dbus_message_unref(reply); 
        std::cout << "End" << std::endl;
      
    }
    
    return DBUS_HANDLER_RESULT_HANDLED;
}

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
    timer.start(1, [=]() {
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
        
    return 0;
}
