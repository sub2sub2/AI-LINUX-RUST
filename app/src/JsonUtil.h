#ifndef JSON_UTIL_H
#define JSON_UTIL_H

#include <string>
#include <vector>
#include <json/json.h>

namespace JsonUtil {
    std::string readFile(const std::string& filepath);
    void writeFile(const std::string& filepath, const std::vector<uint8_t>& data);
    Json::Value createJsonFromImages(const std::vector<std::string>& imageFiles);
    void saveJsonToFile(const std::string& filename, const Json::Value& jsonData);
    Json::Value readJsonFromFile(const std::string& filename);
}

#endif // JSON_UTIL_H
