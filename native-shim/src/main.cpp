#include <iostream>
#include <thread>

#include "app_control_commands.h"
#include "bridge_socket_server.h"
#include "context_engine_client.h"
#include "db_reader.h"

namespace {
constexpr const char* kSocketPath = "/tmp/contextengine-tester-shim.sock";
constexpr const char* kDbConfigPath = "db_config.json";
}

int main() {
    using namespace tester;

    auto appControl = CreateAppControlClient();
    auto subscriber = CreateContextSubscriber();
    auto dbReader = CreateDbReader(kDbConfigPath);

    BridgeSocketServer server(kSocketPath, *appControl, *dbReader);

    // 소켓 서버는 별도 스레드에서 블로킹 실행 — app_control 응답 콜백을 받는 메인(glib)
    // 루프 스레드와 분리해야 한다 (app_control_client.cpp의 NOTE 참고).
    std::thread serverThread([&server] { server.Run(); });

    bool subscribed = subscriber->Subscribe({}, {}, [&server](const ContextEvent& ev) {
        server.BroadcastEvent(ev);
    });

    if (!subscribed) {
        std::cerr << "[contextengine-tester-shim] WARNING: TIDL subscribe가 아직 구현되지 "
                     "않았습니다 (tidl_subscribe_client.cpp의 TODO 참고). 실시간 push는 "
                     "동작하지 않지만 조회/주입/삭제 명령은 정상 동작합니다.\n";
    }

    // TODO: 실제 Tizen 서비스 앱이라면 여기서 glib/EFL 메인 루프를 돌려야 app_control
    // 응답 콜백이 정상적으로 디스패치된다 (예: g_main_loop_run()). 이 파일은 참고용
    // 골격이므로 단순화를 위해 서버 스레드만 join한다.
    serverThread.join();
    return 0;
}
