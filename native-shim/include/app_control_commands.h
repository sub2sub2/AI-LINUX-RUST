#pragma once

#include <memory>
#include <string>

namespace tester {

// contextengine app_control 응답 (docs/interface.md 2절 포맷).
struct CommandResult {
    bool ok = false;
    std::string dataJson;  // ok == true일 때 "data" 필드 원문 JSON
    std::string errorCode;
    std::string errorMessage;
};

// app_control 호출 래퍼. 실제 구현은 src/app_control_client.cpp.
class AppControlClient {
public:
    virtual ~AppControlClient() = default;

    virtual CommandResult GetContext(const std::string& category, const std::string& key) = 0;
    virtual CommandResult SetContext(const std::string& category, const std::string& key,
                                      const std::string& valueJson) = 0;
    virtual CommandResult DeleteContext(const std::string& category, const std::string& key) = 0;
    virtual CommandResult ListContext(const std::string& category) = 0;
    virtual CommandResult ClearTestContext() = 0;
};

std::unique_ptr<AppControlClient> CreateAppControlClient();

}  // namespace tester
