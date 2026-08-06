// TODO(회사 내부 구현 필요): 이 파일은 실제 contextengine TIDL 생성 클라이언트에 연결해야
// 한다. 이 저장소에는 실제 .tidl 정의/생성 코드가 없어 아래는 컴파일되는 골격일 뿐,
// Subscribe()는 항상 false를 반환하는 상태로 두었다.
//
// 채워야 할 것:
//   1. contextengine TIDL 생성 헤더 include (실제 생성 파일명으로 교체)
//   2. subscribe 델리게이트(delegate) 콜백 등록 — 카테고리/키 필터 지원 확인됨
//   3. 델리게이트 콜백에서 ContextEvent로 변환 후 onEvent_ 호출
//      (delete 이벤트 표현 방식은 docs/interface.md 4절 미확정 항목 — 확정되는 대로
//       ev.deleted 세팅 로직 반영)

#include "context_engine_client.h"

namespace tester {

class TidlSubscribeClient : public ContextSubscriber {
public:
    bool Subscribe(const std::vector<std::string>& categories,
                    const std::vector<std::string>& keys, ContextEventCallback onEvent) override {
        onEvent_ = std::move(onEvent);
        (void)categories;
        (void)keys;

        // TODO: 실제 TIDL 생성 클라이언트 인스턴스 생성 및 델리게이트 등록
        //
        // client_ = std::make_unique<GeneratedContextEngineClient>();
        // client_->Subscribe(categories, keys, [this](const GeneratedContextChangedEvent& e) {
        //     ContextEvent ev;
        //     ev.category = e.category;
        //     ev.key = e.key;
        //     ev.valueJson = e.value_json;
        //     ev.deleted = /* e가 delete를 나타내는 방식에 맞춰 세팅 */ false;
        //     ev.timestampMs = e.timestamp;
        //     ev.source = e.source;
        //     onEvent_(ev);
        // });

        return false;  // TODO: 실제 구현 완료 후 true로 변경
    }

    void Unsubscribe() override {
        // TODO: client_->Unsubscribe();
    }

private:
    ContextEventCallback onEvent_;
};

std::unique_ptr<ContextSubscriber> CreateContextSubscriber() {
    return std::make_unique<TidlSubscribeClient>();
}

}  // namespace tester
