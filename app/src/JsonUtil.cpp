#include "JsonUtil.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace JsonUtil {

    std::string readFile(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        std::ostringstream oss;
        oss << file.rdbuf();
        return oss.str();
    }

    void writeFile(const std::string& filepath, const std::vector<uint8_t>& data) {
        std::ofstream file(filepath, std::ios::binary);
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
    }

    Json::Value createJsonFromImages(const std::vector<std::string>& imageFiles) {
        Json::Value jsonData;
        for (const auto& filepath : imageFiles) {
            std::string imageData = readFile(filepath);
            Json::Value imageValue;
            imageValue["data"] = Json::Value(Json::arrayValue);
            for (auto c : imageData) {
                imageValue["data"].append(static_cast<uint8_t>(c));
            }
            jsonData[filepath] = imageValue;
        }
        return jsonData;
    }

    void saveJsonToFile(const std::string& filename, const Json::Value& jsonData) {
        std::ofstream file(filename);
        if (file.is_open()) {
            Json::StreamWriterBuilder writer;
            file << Json::writeString(writer, jsonData);
            file.close();
        } else {
            std::cerr << "Unable to open file " << filename << std::endl;
        }
    }

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
}