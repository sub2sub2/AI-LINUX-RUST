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

#include "Timer.h"
#include "Model.h"

using namespace AIModel::Model;
using namespace AIModel::Utils;

const std::string interfaceName = "org.mca.Model2";
const std::string serviceName = "org.mca.Model2";
const std::string objectPath = "/org/mca/Model2";
const std::string modelPath = "/home/hyunsub/workspace/hyunsub/AI-LINUX-RUST/model/weights/mnist.onnx";

std::unique_ptr<ModelInstance> mModel = std::make_unique<ModelInstance>(modelPath);
Timer timer;

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
        
        /*
        std::vector<int64_t> output_shape = {1, 1};
        std::vector<int64_t> input_shape = {1, 4};
        mModel->set_input_size(input_ shape);
        mModel->set_output_size(output_shape);

        std::vector<const char *> input_name = {"input"};
        mModel->set_input_name(input_name);

        const char* response = mModel->infernce(input_data).c_str();
        DBusMessage* reply = dbus_message_new_method_return(message);
        dbus_message_append_args(reply, DBUS_TYPE_STRING, &response, DBUS_TYPE_INVALID);
        dbus_connection_send(connection, reply, NULL);
        dbus_message_unref(reply);      */        
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
