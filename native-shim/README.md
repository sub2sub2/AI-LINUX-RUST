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
- [ ] `CMakeLists.txt` / `packaging/*.spec`: TIDL 생성 라이브러리 BuildRequires/링크 추가
- [ ] `tizen-manifest.xml`: package id, privilege 목록을 실제 값으로 확정
- [ ] `.gbs.conf`: 사내 Tizen OBS/저장소 URL로 교체
- [ ] `../docs/interface.md` 4절의 미확인 항목 확정 (특히 delete 이벤트 표현 방식)

## GBS로 빌드하기

이 프로젝트는 GBS(플랫폼/이미지 빌드) 방식을 전제로 구성했다:

- `CMakeLists.txt` — 일반 CMake. GBS가 spec의 `%cmake` 매크로로 그대로 구동한다
- `packaging/contextengine-tester-shim.spec` — RPM spec. `%install`에서 바이너리(via
  `%make_install`), `tizen-manifest.xml`, `db_config.json`을 배치한다
- `packaging/contextengine-tester-shim.manifest` — SMACK 매니페스트 (최소 placeholder)
- `tizen-manifest.xml` — 서비스 앱 매니페스트 (package id/privilege는 TODO)

```sh
gbs build -A <arch> --include-all   # 예: armv7l, aarch64
```

TIDL 생성 라이브러리를 아직 링크하지 않았으므로(README 위 체크리스트), 현재 상태로는
`gbs build`도 링크 단계에서 실패한다. 그 전까지는 `cmake . && make`로 나머지(app_control,
소켓 서버, DB 조회)까지는 컴파일 여부를 로컬에서 미리 확인할 수 있다 — 단, glib/app_control/
sqlite3 개발 패키지가 있는 환경(예: sbi/sysroot)에서만.

## 프로토콜

bridge-server와의 로컬 소켓 프로토콜(`/tmp/contextengine-tester-shim.sock`, 줄바꿈 구분
JSON)은 `../docs/interface.md` 3절 참고. tester 내부 구현 세부사항이라 자유롭게 바꿔도 된다.
