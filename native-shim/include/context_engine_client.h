#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace tester {

// TIDL subscribe 콜백으로부터 전달되는 context 변경 이벤트.
struct ContextEvent {
    std::string category;
    std::string key;
    std::string valueJson;  // JSON 인코딩된 값. deleted == true면 비어있음.
    bool deleted = false;
    long long timestampMs = 0;
    std::string source;  // plugin id, 또는 tester가 주입한 경우 "test-injected"
};

using ContextEventCallback = std::function<void(const ContextEvent&)>;

// TIDL 구독 클라이언트 추상화.
// 실제 구현(src/tidl_subscribe_client.cpp)은 contextengine이 제공하는 TIDL 생성 스텁을
// 감싸야 한다 — 해당 스텁은 내부 코드라 이 저장소에는 없다. TODO 주석을 따라 채울 것.
class ContextSubscriber {
public:
    virtual ~ContextSubscriber() = default;

    // categories/keys가 모두 비어있으면 전체 구독.
    virtual bool Subscribe(const std::vector<std::string>& categories,
                            const std::vector<std::string>& keys,
                            ContextEventCallback onEvent) = 0;
    virtual void Unsubscribe() = 0;
};

std::unique_ptr<ContextSubscriber> CreateContextSubscriber();

}  // namespace tester
