# contextengine-tester 구조 및 동작 방식

`contextengine-tester`는 contextengine 데몬(Tizen 기반 C++, plugin이 주기적으로 보고하는
Context 데이터를 DB에 저장하고 구독 앱에 전달)을 시각적으로 테스트하기 위한 web 도구다.
CLI/tc만으로는 확인하기 어려운 것들 — 실시간 데이터 흐름, 임의 데이터 주입/삭제로 구독
경로 검증, plugin이 실제로 데이터를 제대로 보고하고 있는지, DB에 뭐가 들어있는지 — 을
브라우저에서 바로 확인할 수 있게 한다.

데몬 쪽과 주고받는 정확한 프로토콜(app_control 커맨드, TIDL 구독 계약)은
[`interface.md`](./interface.md)에 별도로 정리되어 있다. 이 문서는 tester **자체의**
내부 구조와 각 기능이 실제로 어떻게 동작하는지를 다룬다.

## 전체 구조

```
[Tizen 디바이스]
  contextengine
      │ TIDL subscribe(push, 카테고리/키 필터)   │ app_control(get/set/delete-context 등)
      ▼                                          ▼
                     native-shim (C++, Tizen 서비스 앱)
                              │ SQLite 파일 직접 read-only 오픈 (db_reader)
                              │
                              │ 로컬 유닉스 소켓, 줄바꿈 구분 JSON
                              ▼
                     bridge-server (Node.js)
                      - REST API
                      - WebSocket (실시간 push 중계)
                      - web/ 정적 파일 서빙
                              │
                        sdb forward (또는 동일 네트워크)
                              ▼
                     web 프론트엔드 (브라우저)
```

세 컴포넌트는 각자 독립적으로 교체 가능하도록 분리했다:
- **native-shim**만 실제 Tizen/데몬에 의존한다 (TIDL, app_control, SQLite 파일 경로)
- **bridge-server**는 native-shim과 로컬 소켓으로만 통신 — `SHIM_MODE=mock`이면
  native-shim 없이도 개발/테스트 가능 (아래 "mock 모드" 참고)
- **web**은 bridge-server의 REST/WebSocket만 알면 됨

## 컴포넌트

### native-shim (`native-shim/`)

Tizen C++ 서비스 앱. 세 가지 창구를 로컬 소켓(`/tmp/contextengine-tester-shim.sock`)
뒤로 감춘다:

| 창구 | 파일 | 상태 |
|---|---|---|
| TIDL 구독 (실시간 push) | `src/tidl_subscribe_client.cpp` | **스텁** — 실제 TIDL 생성 코드 연결 필요 |
| app_control 호출 | `src/app_control_client.cpp` | Tizen 공개 API만 사용, 동작 예상 |
| SQLite 직접 조회 | `src/db_reader.cpp` | Tizen 공개 API만 사용, 동작 예상 |
| 소켓 서버(위 셋을 중계) | `src/bridge_socket_server.cpp` | 동작 예상 |

빌드는 GBS 기반으로 구성했다 (`tizen-manifest.xml`, `packaging/*.spec`,
`.gbs.conf`). 채워야 할 것들은 `native-shim/README.md`의 체크리스트 참고.

### bridge-server (`bridge-server/`)

Node.js. native-shim의 소켓 클라이언트 역할과 REST/WebSocket 서버 역할을 겸한다.

- `src/shimClient.js` — 실제 native-shim에 연결 (`SHIM_MODE=socket`)
- `src/mockShim.js` + `src/mockDb.js` — native-shim 없이 개발/테스트하기 위한 인메모리
  mock (`SHIM_MODE=mock`, 기본값)
- `src/catalogStatus.js` — 카탈로그 vs 실제 context 대조 로직 (순수 함수, 데몬과 무관)
- `src/index.js` — Express + `ws`로 REST/WebSocket 서버, `web/` 정적 파일 서빙

환경변수: `PORT`(기본 8787), `SHIM_MODE`(`mock` | `socket`, 기본 `mock`),
`SHIM_SOCKET_PATH`(기본 `/tmp/contextengine-tester-shim.sock`).

### web (`web/`)

프레임워크 없는 순수 HTML/CSS/JS 3개 파일. bridge-server가 정적으로 서빙한다.

## 기능별 동작 방식

### 1. 실시간 context 조회

- 최초 로드 시 `GET /api/context`로 스냅샷을 가져와 테이블을 채움
- 이후 변경은 **polling이 아니라 push**: native-shim이 TIDL로 contextengine을
  구독(전체 카테고리/키)해서 변경이 생기는 즉시 로컬 소켓으로 bridge-server에 전달하고,
  bridge-server는 이를 연결된 모든 브라우저에 WebSocket(`/ws`)으로 그대로 broadcast
- 브라우저는 `context_changed`/`context_deleted` 이벤트를 받아 테이블 행을 갱신하고
  잠깐 하이라이트(flash) 표시

### 2. 데이터 주입 / 삭제

- 주입: 폼에 category/key/value(JSON 또는 문자열) 입력 → `POST /api/context` →
  native-shim이 `set-context` app_control 커맨드 호출 → **실제 plugin이 보고하는 것과
  동일한 경로**(DB 저장 + 구독자 push)를 태움. 내부적으로 `source: "test-injected"`로
  마킹되어 실제 plugin 데이터와 구분됨
- 삭제: 행의 "삭제" 버튼 → `DELETE /api/context/:category/:key` → `delete-context`
- 일괄 정리: "test-injected 데이터 일괄 삭제" 버튼 → `POST /api/context/clear-test` →
  `source: "test-injected"`로 마킹된 것만 지움 (실제 plugin 데이터는 안 건드림)

### 3. 카탈로그 로드 상태

plugin이 "이런 데이터를 주겠다"고 선언한 정의(카탈로그)와, 실제로 들어오고 있는 데이터를
대조해서 빠진 게 없는지 보여주는 화면.

- `GET /api/catalog` 호출 시 bridge-server가 `list-catalog`(정의)와 `list-context`(실측)
  둘 다 조회해서 `category`+`key`로 조인 (`catalogStatus.js`)
- 상태 판정:
  - **missing**: 카탈로그엔 있는데 매칭되는 context가 한 번도 없었음
  - **stale**: context는 있는데, `report_interval_ms`가 있고 마지막 보고로부터 그 **3배**
    이상 지남
  - **ok**: 매칭되고 신선함
- 화면은 5초마다 자동 새로고침 + 수동 새로고침 버튼

### 4. DB 데이터 보기 (브라우즈 + 검색)

SQL을 몰라도 쓸 수 있도록 설계했다.

- DB 선택 → 테이블 선택 → **자동으로 `SELECT * FROM <table> LIMIT 500`을 실행**해서
  전체 데이터를 바로 보여줌 (직접 쿼리 작성 불필요)
- 검색창에 입력하면 **이미 불러온 행들(최대 500행) 안에서, 모든 컬럼을 대상으로 대소문자
  무시 부분 일치**로 클라이언트(JS)에서 즉시 필터링 — 서버에 재요청 안 함
- 결과 하단에 필터링된 행 수 / 전체 로드된 행 수 / truncated 여부를 표시
- **알려진 제약**: 검색은 이미 로드된 최대 500행 안에서만 동작한다. 테이블에 500행보다
  많은 데이터가 있으면(하단에 "DB에 더 많은 행이 있어 일부만 불러왔습니다" 표시) 그
  이후 행은 검색 대상에 아예 포함되지 않는다. DB 전체를 대상으로 검색하려면 서버 쪽에서
  `WHERE ... LIKE` 쿼리를 만들어 보내야 하는데, 현재는 구현하지 않았다 (필요해지면
  `bridge-server`의 `/api/db/:dbId/query`를 그대로 두고 검색어를 SQL로 변환하는
  얇은 레이어만 추가하면 됨 — UX는 안 바뀜)
- 안전장치: native-shim은 SQLite 연결을 `SQLITE_OPEN_READONLY`로 열고, 추가로
  `sqlite3_stmt_readonly()`로 재검증해서 SELECT 외 쿼리를 거부한다 (`interface.md` 2.5절)
- "고급: SQL 직접 입력" 접이식 섹션에서 원하면 직접 SQL을 쓸 수도 있다 (기본은 접힘).
  이 경로도 결과는 같은 검색/필터 로직을 탄다

## mock 모드

`SHIM_MODE=mock`(기본값)으로 실행하면 native-shim/실제 디바이스 없이 전체 기능을 테스트할
수 있다:

```sh
cd bridge-server
npm install
PORT=8787 SHIM_MODE=mock node src/index.js
# http://localhost:8787 접속
```

- `mockShim.js`: context store를 인메모리로 흉내내고, `Device/battery`를 4초마다 갱신하는
  데모 ticker, 그리고 카탈로그 3종 데모(ok/stale/missing 각 1개)를 미리 채워둠
- `mockDb.js`: Node 22 내장 `node:sqlite`로 임시 SQLite 파일을 만들어 `plugins`,
  `context_log` 두 테이블에 샘플 데이터를 채우고, 실제 SELECT 실행 + 쓰기 차단까지
  진짜로 검증 가능

실제 디바이스에 붙이려면 `SHIM_MODE=socket`으로 실행 (native-shim이 먼저 떠 있어야 함).

## 알려진 제약사항 / TODO

- `native-shim`의 TIDL 구독은 스텁 상태 — 연결 전까지는 실시간 push가 동작하지 않음
  (`app_control` 기반 기능은 정상 동작)
- DB 검색은 로드된 최대 500행 내에서만 동작 (위 4번 참고)
- `list-catalog` 실제 필드 스키마는 가정치 (`interface.md` 2.1절)
- delete 이벤트의 TIDL 콜백 payload 표현 방식 미확정 (`value: null` vs `deleted: true`)
- `native-shim`의 GBS 빌드는 TIDL 라이브러리 링크가 채워지기 전까지 완주하지 못함

## 향후 기능 백로그 (미착수, 우선순위 논의용)

- 구독자(subscriber) 앱 목록/상태
- plugin 로드 상태(크래시/재시작 이력)
- context 변경 이력/타임라인 뷰 (검색/필터 가능한 append-only 로그)
- context 스냅샷 export/import (재현 가능한 테스트 시나리오)
- missing/stale 전환 시 알림
- DB 검색을 서버사이드 `LIKE` 쿼리로 확장해서 500행 캡 제거
