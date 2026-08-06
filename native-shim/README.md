# native-shim

contextengine-tester의 디바이스 측 컴포넌트. Tizen 서비스 앱으로 동작하며:

1. TIDL로 contextengine을 구독해서 실시간 push를 받는다
2. app_control으로 조회/주입/삭제 명령을 실행한다
3. SQLite DB 파일을 read-only로 직접 열어 조회한다 (app_control/TIDL과 무관, `db_reader.cpp`)
4. 위 셋을 bridge-server(Node.js)에 로컬 유닉스 소켓으로 중계한다

## 빌드 가능 여부

**이 상태로는 빌드되지 않는다.** `src/tidl_subscribe_client.cpp`는 contextengine의 실제
TIDL 생성 코드(내부 코드, 이 저장소 범위 밖)에 연결해야 하는 자리표시자(stub)다. 나머지
(app_control 호출, 소켓 서버, DB 직접 쿼리)는 Tizen/SQLite 공개 API만 사용하므로 그대로
동작할 것으로 예상되나 실제 디바이스/에뮬레이터에서 검증되지 않았다.

## 채워야 할 것 (회사 내부 작업)

- [ ] `src/tidl_subscribe_client.cpp`: 실제 TIDL 생성 클라이언트로 교체
- [ ] `src/app_control_client.cpp`: `kContextEngineAppId`, `kOperation` 상수를 실제 값으로 교체
- [ ] `db_config.json`: 실제 SQLite DB 파일 경로로 교체
- [ ] `CMakeLists.txt`: TIDL 생성 라이브러리 링크 추가
- [ ] `../docs/interface.md` 4절의 미확인 항목 확정 (특히 delete 이벤트 표현 방식)

## 프로토콜

bridge-server와의 로컬 소켓 프로토콜(`/tmp/contextengine-tester-shim.sock`, 줄바꿈 구분
JSON)은 `../docs/interface.md` 3절 참고. tester 내부 구현 세부사항이라 자유롭게 바꿔도 된다.
