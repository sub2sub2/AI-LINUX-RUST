#pragma once

#include <memory>
#include <string>

#include "app_control_commands.h"  // CommandResult 재사용

namespace tester {

// contextengine이 쓰는 SQLite DB들을 read-only로 직접 조회하는 컴포넌트.
// app_control/TIDL과 완전히 별개 경로 — DB 파일이 직접 접근 가능하다는 전제 하에 동작한다
// (docs/interface.md 2.5절). 데몬 쪽 인터페이스 변경이 필요 없다.
class DbReader {
public:
    virtual ~DbReader() = default;

    // data: { "databases": [{ "id", "label", "path" }] }
    virtual CommandResult ListDatabases() = 0;

    // data: { "tables": [{ "name", "columns": [{ "name", "type" }] }] }
    virtual CommandResult GetSchema(const std::string& dbId) = 0;

    // SELECT 전용. data: { "columns": [...], "rows": [[...]], "truncated": bool }
    virtual CommandResult Query(const std::string& dbId, const std::string& sql) = 0;
};

// configPath: db_config.json 경로 (db_id -> 파일 경로 매핑, 실제 값은 회사에서 채워야 함).
std::unique_ptr<DbReader> CreateDbReader(const std::string& configPath);

}  // namespace tester
