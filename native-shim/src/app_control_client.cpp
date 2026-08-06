// app_control 기반 contextengine 명령 호출 구현.
//
// Tizen 공개 API(capi-appfw-app-control)만 사용하므로 그대로 동작할 것으로 예상되지만
// 실제 디바이스/에뮬레이터에서 검증되지 않았다. 아래 두 상수는 실제 값으로 교체해야 한다.

#include "app_control_commands.h"

#include <app_control.h>

#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <mutex>

namespace tester {

namespace {

// TODO: 실제 contextengine app id / operation 문자열로 교체
constexpr const char* kContextEngineAppId = "org.tizen.contextengine";
constexpr const char* kOperation =
    "http://tizen.org/appcontrol/operation/org.tizen.contextengine.control";

struct ReplyContext {
    CommandResult* result;
    std::mutex* mtx;
    std::condition_variable* cv;
    bool* done;
};

void OnReply(app_control_h /*request*/, app_control_h reply, app_control_result_e /*result*/,
             void* userData) {
    auto* ctx = static_cast<ReplyContext*>(userData);

    char* statusRaw = nullptr;
    char* dataRaw = nullptr;
    app_control_get_extra_data(reply, "status", &statusRaw);
    app_control_get_extra_data(reply, "data", &dataRaw);

    std::string status = statusRaw ? statusRaw : "error";
    if (status == "ok") {
        ctx->result->ok = true;
        ctx->result->dataJson = dataRaw ? dataRaw : "{}";
    } else {
        char* code = nullptr;
        char* message = nullptr;
        app_control_get_extra_data(reply, "code", &code);
        app_control_get_extra_data(reply, "message", &message);
        ctx->result->ok = false;
        ctx->result->errorCode = code ? code : "unknown";
        ctx->result->errorMessage = message ? message : "no message";
        free(code);
        free(message);
    }
    free(statusRaw);
    free(dataRaw);

    {
        std::lock_guard<std::mutex> lock(*ctx->mtx);
        *ctx->done = true;
    }
    ctx->cv->notify_one();
}

}  // namespace

class AppControlClientImpl : public AppControlClient {
public:
    CommandResult GetContext(const std::string& category, const std::string& key) override {
        return Invoke("get-context", {category, key});
    }
    CommandResult SetContext(const std::string& category, const std::string& key,
                              const std::string& valueJson) override {
        return Invoke("set-context", {category, key, valueJson});
    }
    CommandResult DeleteContext(const std::string& category, const std::string& key) override {
        return Invoke("delete-context", {category, key});
    }
    CommandResult ListContext(const std::string& category) override {
        return Invoke("list-context", {category});
    }
    CommandResult ClearTestContext() override { return Invoke("clear-test-context", {}); }

private:
    CommandResult Invoke(const std::string& method, const std::vector<std::string>& params) {
        CommandResult result;

        app_control_h handle = nullptr;
        if (app_control_create(&handle) != APP_CONTROL_ERROR_NONE) {
            result.errorCode = "app_control_create_failed";
            result.errorMessage = "failed to create app_control handle";
            return result;
        }

        app_control_set_app_id(handle, kContextEngineAppId);
        app_control_set_operation(handle, kOperation);
        app_control_add_extra_data(handle, "method", method.c_str());
        for (size_t i = 0; i < params.size(); ++i) {
            app_control_add_extra_data(handle, ("param" + std::to_string(i)).c_str(),
                                        params[i].c_str());
        }

        std::mutex mtx;
        std::condition_variable cv;
        bool done = false;
        ReplyContext replyCtx{&result, &mtx, &cv, &done};

        app_control_send_launch_request(handle, OnReply, &replyCtx);
        app_control_destroy(handle);

        // NOTE: OnReply는 Tizen 메인 루프(glib/EFL) 스레드에서 호출된다. 이 Invoke()를
        // 메인 루프를 돌리는 바로 그 스레드에서 부르면 데드락이 난다 — 반드시 별도 워커
        // 스레드(예: BridgeSocketServer::Run() 스레드)에서 호출할 것.
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(5), [&] { return done; });
        if (!done) {
            result.ok = false;
            result.errorCode = "timeout";
            result.errorMessage = "app_control reply timed out";
        }

        return result;
    }
};

std::unique_ptr<AppControlClient> CreateAppControlClient() {
    return std::make_unique<AppControlClientImpl>();
}

}  // namespace tester
