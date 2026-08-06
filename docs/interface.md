# contextengine 외부 인터페이스 계약 (contextengine-tester 연동용)

이 문서는 `contextengine-tester`(시각적 테스트 도구)가 contextengine 데몬과 통신하기 위해 필요한 인터페이스를 정의한다. 데몬 측 구현 기준으로 사용한다.

두 개의 독립적인 채널이 있다.

1. **TIDL** — 실시간 구독(push). sync call + 콜백(delegate) 기반.
2. **app_control** — 단발성 명령(request/response). `method + params` 형태.

---

## 1. TIDL: 구독 (실시간 push)

- subscribe 호출 시 **카테고리/키 필터**를 지정할 수 있음 (지원 확인됨).
- 필터에 매칭되는 context가 변경될 때마다 콜백(delegate)으로 전달됨.
- 정확한 TIDL 메서드 시그니처는 실제 `.tidl` 정의를 따른다 — 아래는 tester가 기대하는 **개념적 계약**이다.

```
subscribe(filter: { categories?: string[], keys?: string[] }, callback: OnContextChanged)
unsubscribe(subscriptionId)

OnContextChanged(event: {
  category: string,
  key: string,
  value: <JSON>,
  timestamp: <epoch ms>,
  source: string        // 예: plugin id, 또는 "test-injected"
})
```

- `filter`를 비워두면(또는 전체 와일드카드) 모든 context 변경을 수신 — tester의 기본 동작.
- 삭제(delete)도 이 콜백을 통해 알려져야 함 (예: `value: null` 또는 별도 `deleted: true` 필드로 표현 — **데몬 측 기존 컨벤션에 맞추고, tester가 그에 맞게 파싱**).

## 2. app_control: 명령 (request/response)

기존 컨벤션:
```
app_control context-engine <method> <param1> <param2> ...
```
예: `app_control context-engine get-context Setting language`

응답 포맷 (기존 확정된 포맷 가정, 추후 실제 포맷에 맞춰 조정):
```json
// 성공
{ "status": "ok", "data": { ... } }
// 실패
{ "status": "error", "code": "...", "message": "..." }
```

### 2.1 기존 커맨드 (변경 없음)
- `get-context <category> <key>` — 현재 값 조회
- `list-catalog [plugin_id]` — 카탈로그(plugin이 어떤 category/key의 데이터를 제공하기로
  선언했는지에 대한 정의, DB에 저장됨) 조회. **스키마는 실제 값을 확인하지 못해 아래는
  가정치다 — 실제 필드명/구조가 다르면 bridge-server의 `/api/catalog` 매핑만 맞춰 고치면 됨.**

  ```json
  {
    "status": "ok",
    "data": {
      "items": [
        {
          "plugin_id": "battery-plugin",
          "category": "Device",
          "key": "battery",
          "data_type": "object",
          "description": "배터리 상태",
          "report_interval_ms": 5000
        }
      ]
    }
  }
  ```
  - `report_interval_ms`가 없다면(스키마에 없으면) tester는 "존재 여부"만으로 상태를 판단하고
    stale 판정은 생략한다.

### 2.2 신규 커맨드 (테스트 전용, 이번에 추가 필요)

**`set-context <category> <key> <value>`**
- `value`: JSON-encoded 문자열
- 실제 plugin이 보고한 것과 **동일한 코드 경로**(DB 저장 + 구독자 push)를 태워야 함
- 내부적으로 `source: "test-injected"` 마커를 남길 것 (실제 plugin 데이터와 구분, clear-test-context에서 활용)

**`delete-context <category> <key>`**
- 특정 context 값 제거. 정상 delete/notify 경로를 타서 구독자에게 삭제가 전파되어야 함

**`list-context [category]`** (선택, 권장)
- 현재 존재하는 category/key 목록 열거. tester UI가 하드코딩 없이 탐색 가능하게 함

**`clear-test-context`** (선택, 권장)
- `source: "test-injected"`로 마킹된 데이터 일괄 삭제. 테스트 세션 정리용

### 2.3 보안/빌드 가드 (필수 제안)
- `set-context` / `delete-context` / `clear-test-context`는 `CONTEXTENGINE_TEST_BUILD` 빌드 플래그로 감싸서 release 빌드에서 제외
- 신규 내부 전용 privilege(예: `http://tizen.org/privilege/contextengine.testcontrol`) 정의, 테스트 빌드에서도 서명된 내부 앱만 호출 가능하도록 제한

### 2.4 카탈로그 로드 상태 판단 (tester 내부 로직, 데몬 변경 불필요)

tester는 `list-catalog`(정의)와 `list-context`(실측)를 각각 조회해서 bridge-server에서
`category`+`key`로 조인한 뒤 항목별 상태를 계산한다:

- **missing**: catalog에는 있는데 매칭되는 context가 없음 — plugin이 아예 보고 안 하고 있음
- **stale**: context는 있는데 `report_interval_ms`가 있고, 마지막 보고 시각이 그 3배 이상
  지남 — 한동안 안 들어옴
- **ok**: 매칭되고 신선함

이 로직은 데몬 인터페이스와 무관한 tester 내부 구현이라 자유롭게 조정 가능.

---

## 2.5 DB 직접 쿼리 (신규, app_control/TIDL과 완전히 별개 경로)

contextengine이 사용하는 SQLite DB들이 **파일 경로로 직접 접근 가능**하다는 것을 확인했다
(사용자 확인 완료). 따라서 이 기능은 app_control이나 TIDL을 전혀 거치지 않고,
native-shim이 DB 파일을 직접 read-only로 열어서 조회한다 — **데몬 쪽 인터페이스 변경이
필요 없다.**

- `native-shim/db_config.json`에 db_id → 파일 경로 매핑을 채워 넣기만 하면 됨 (경로는
  회사 내부 정보라 이 저장소에는 placeholder만 있음)
- 쿼리는 SELECT만 허용 (사용자 확인 완료) — `SQLITE_OPEN_READONLY`로 연결을 열고, 준비된
  statement에 대해 `sqlite3_stmt_readonly()`로 재검증
- 결과 행 수 상한(1000행) 적용

자세한 프로토콜은 `native-shim/README.md`와 `bridge-server` 소스의 `/api/db/*` 참고.

---

## 3. tester 쪽 아키텍처 요약 (참고)

```
contextengine ── TIDL subscribe(push) ──► native-shim (Tizen C++)
              ◄── app_control ───────────┘
                                              │ local socket (JSON, tester 내부 프로토콜)
                                              ▼
                                        bridge-server (Node.js)
                                         - REST API, WebSocket, 정적 파일 서빙
                                              │
                                        sdb forward
                                              ▼
                                        web 프론트엔드 (host PC 브라우저)
```

native-shim ↔ bridge-server 간 로컬 소켓 프로토콜은 tester 내부 구현 세부사항이며 이 계약과 무관 (자유롭게 변경 가능).

## 4. 확인 상태
- [x] 응답 포맷: `{status, data}` / `{status, error}` — 기존 포맷으로 가정, 추후 실제 값으로 교체
- [x] TIDL subscribe 필터링: 카테고리/키 지원 확인됨
- [ ] delete 시 콜백 payload 표현 방식 (`value: null` vs `deleted: true`) — 데몬 구현 시 확정 필요
- [ ] `list-context` / `clear-test-context` 채택 여부 — 선택 사항, 필요 없으면 tester 쪽에서 우회 가능
- [x] `list-catalog` 커맨드는 이미 존재함 (스키마는 가정치, 실제 필드명 확인 필요)
- [x] DB들이 파일 경로로 직접 접근 가능한 SQLite인지 확인됨 → app_control/TIDL 변경 불필요
- [x] DB 쿼리 권한은 SELECT만 허용하기로 확인됨
