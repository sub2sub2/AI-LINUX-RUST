#pragma once

#include <string>

#include "app_control_commands.h"
#include "context_engine_client.h"
#include "db_reader.h"

namespace tester {

// bridge-server(Node.js)와 통신하는 로컬 유닉스 도메인 소켓 서버.
// 프로토콜: 줄바꿈으로 구분된 JSON (newline-delimited JSON). docs/interface.md 3절 참고.
// tester 내부 프로토콜이므로 자유롭게 바꿔도 된다.
class BridgeSocketServer {
public:
    BridgeSocketServer(std::string socketPath, AppControlClient& appControl, DbReader& dbReader);
    ~BridgeSocketServer();

    // 블로킹. 별도 스레드에서 호출할 것 (app_control 응답 콜백을 받는 메인 루프 스레드와
    // 분리해야 함 — app_control_client.cpp의 NOTE 참고).
    void Run();
    void Stop();

    // TIDL 구독 콜백에서 호출 — 연결된 bridge-server로 이벤트를 push.
    void BroadcastEvent(const ContextEvent& event);

private:
    std::string socketPath_;
    AppControlClient& appControl_;
    DbReader& dbReader_;
    int listenFd_ = -1;
    int clientFd_ = -1;
    bool running_ = false;
};

}  // namespace tester
