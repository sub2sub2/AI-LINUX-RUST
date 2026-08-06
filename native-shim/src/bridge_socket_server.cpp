#include "bridge_socket_server.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <nlohmann/json.hpp>

namespace tester {

using json = nlohmann::json;

namespace {

std::string ReadLine(int fd, bool& eof) {
    std::string line;
    char c;
    while (true) {
        ssize_t n = read(fd, &c, 1);
        if (n <= 0) {
            eof = true;
            break;
        }
        if (c == '\n') break;
        line.push_back(c);
    }
    return line;
}

void WriteLine(int fd, const std::string& line) {
    std::string out = line + "\n";
    ::write(fd, out.data(), out.size());
}

CommandResult Dispatch(AppControlClient& client, const std::string& method, const json& params) {
    if (method == "get-context") {
        return client.GetContext(params.value("category", ""), params.value("key", ""));
    }
    if (method == "set-context") {
        json v = params.value("value", json());
        return client.SetContext(params.value("category", ""), params.value("key", ""), v.dump());
    }
    if (method == "delete-context") {
        return client.DeleteContext(params.value("category", ""), params.value("key", ""));
    }
    if (method == "list-context") {
        return client.ListContext(params.value("category", ""));
    }
    if (method == "clear-test-context") {
        return client.ClearTestContext();
    }
    CommandResult r;
    r.ok = false;
    r.errorCode = "unknown_method";
    r.errorMessage = "unsupported method: " + method;
    return r;
}

}  // namespace

BridgeSocketServer::BridgeSocketServer(std::string socketPath, AppControlClient& appControl)
    : socketPath_(std::move(socketPath)), appControl_(appControl) {}

BridgeSocketServer::~BridgeSocketServer() { Stop(); }

void BridgeSocketServer::Run() {
    ::unlink(socketPath_.c_str());

    listenFd_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socketPath_.c_str(), sizeof(addr.sun_path) - 1);

    ::bind(listenFd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    ::listen(listenFd_, 1);

    running_ = true;
    while (running_) {
        int clientFd = ::accept(listenFd_, nullptr, nullptr);
        if (clientFd < 0) continue;
        clientFd_ = clientFd;

        bool eof = false;
        while (running_ && !eof) {
            std::string line = ReadLine(clientFd_, eof);
            if (line.empty()) continue;

            json req;
            try {
                req = json::parse(line);
            } catch (...) {
                continue;  // 잘못된 JSON 라인은 무시
            }
            if (req.value("type", "") != "request") continue;

            std::string id = req.value("id", "");
            std::string method = req.value("method", "");
            json params = req.value("params", json::object());

            CommandResult result = Dispatch(appControl_, method, params);

            json resp;
            resp["type"] = "response";
            resp["id"] = id;
            if (result.ok) {
                resp["status"] = "ok";
                try {
                    resp["data"] = json::parse(result.dataJson.empty() ? "{}" : result.dataJson);
                } catch (...) {
                    resp["data"] = result.dataJson;
                }
            } else {
                resp["status"] = "error";
                resp["code"] = result.errorCode;
                resp["message"] = result.errorMessage;
            }
            WriteLine(clientFd_, resp.dump());
        }

        ::close(clientFd_);
        clientFd_ = -1;
    }
}

void BridgeSocketServer::Stop() {
    running_ = false;
    if (clientFd_ >= 0) ::close(clientFd_);
    if (listenFd_ >= 0) ::close(listenFd_);
}

void BridgeSocketServer::BroadcastEvent(const ContextEvent& event) {
    // NOTE: 참고용 골격 수준의 구현 — clientFd_는 이 함수(TIDL 콜백 스레드)와 Run()의
    // accept 루프(소켓 서버 스레드)에서 함께 접근된다. 프로덕션에서는 clientFd_ 접근에
    // mutex를 추가할 것.
    if (clientFd_ < 0) return;

    json ev;
    ev["type"] = "event";
    ev["event"] = event.deleted ? "context_deleted" : "context_changed";
    ev["category"] = event.category;
    ev["key"] = event.key;
    if (!event.deleted) {
        try {
            ev["value"] = event.valueJson.empty() ? json() : json::parse(event.valueJson);
        } catch (...) {
            ev["value"] = event.valueJson;
        }
    }
    ev["timestamp"] = event.timestampMs;
    ev["source"] = event.source;

    WriteLine(clientFd_, ev.dump());
}

}  // namespace tester
