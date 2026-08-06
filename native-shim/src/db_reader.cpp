// SQLite 파일을 read-only로 직접 열어 조회하는 구현.
//
// app_control/TIDL 스텁과 달리 Tizen 공개 SQLite API만 사용하므로 사내 전용 의존성이 없다 —
// db_config.json에 실제 파일 경로만 채우면 그대로 동작할 것으로 예상되나, 실제
// 디바이스/에뮬레이터에서 검증되지 않았다.

#include "db_reader.h"

#include <sqlite3.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>

namespace tester {

using json = nlohmann::json;

namespace {

constexpr int kRowLimit = 1000;

CommandResult MakeOk(const json& data) {
    CommandResult r;
    r.ok = true;
    r.dataJson = data.dump();
    return r;
}

CommandResult MakeErr(const std::string& code, const std::string& message) {
    CommandResult r;
    r.ok = false;
    r.errorCode = code;
    r.errorMessage = message;
    return r;
}

}  // namespace

class DbReaderImpl : public DbReader {
public:
    explicit DbReaderImpl(const std::string& configPath) { LoadConfig(configPath); }

    CommandResult ListDatabases() override {
        json arr = json::array();
        for (const auto& db : databases_) {
            arr.push_back({{"id", db.id}, {"label", db.label}, {"path", db.path}});
        }
        return MakeOk({{"databases", arr}});
    }

    CommandResult GetSchema(const std::string& dbId) override {
        const DbEntry* entry = Find(dbId);
        if (!entry) return MakeErr("db_not_found", "unknown db id: " + dbId);

        sqlite3* db = nullptr;
        if (sqlite3_open_v2(entry->path.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) !=
            SQLITE_OK) {
            std::string msg = db ? sqlite3_errmsg(db) : "failed to open db";
            if (db) sqlite3_close(db);
            return MakeErr("open_failed", msg);
        }

        json tables = json::array();
        sqlite3_stmt* stmt = nullptr;
        const char* listTablesSql =
            "SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%'";
        if (sqlite3_prepare_v2(db, listTablesSql, -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::string tableName =
                    reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
                tables.push_back({{"name", tableName}, {"columns", ColumnsOf(db, tableName)}});
            }
        }
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        return MakeOk({{"tables", tables}});
    }

    CommandResult Query(const std::string& dbId, const std::string& sql) override {
        const DbEntry* entry = Find(dbId);
        if (!entry) return MakeErr("db_not_found", "unknown db id: " + dbId);

        sqlite3* db = nullptr;
        // SQLITE_OPEN_READONLY: OS/SQLite 레벨에서 쓰기 자체를 원천 차단 (1차 방어선).
        if (sqlite3_open_v2(entry->path.c_str(), &db, SQLITE_OPEN_READONLY, nullptr) !=
            SQLITE_OK) {
            std::string msg = db ? sqlite3_errmsg(db) : "failed to open db";
            if (db) sqlite3_close(db);
            return MakeErr("open_failed", msg);
        }
        sqlite3_busy_timeout(db, 2000);

        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::string msg = sqlite3_errmsg(db);
            sqlite3_close(db);
            return MakeErr("prepare_failed", msg);
        }

        // 2차 방어선: 문자열 접두어 매칭이 아니라 실제 준비된 statement가 읽기 전용인지
        // 재검증. PRAGMA/ATTACH 등 문자열 매칭으로는 걸러지지 않는 우회 케이스도 여기서 잡힘.
        if (!sqlite3_stmt_readonly(stmt)) {
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return MakeErr("not_readonly", "only read-only (SELECT) queries are allowed");
        }

        json columns = json::array();
        int columnCount = sqlite3_column_count(stmt);
        for (int i = 0; i < columnCount; ++i) {
            columns.push_back(sqlite3_column_name(stmt, i));
        }

        json rows = json::array();
        bool truncated = false;
        int rc;
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            if (static_cast<int>(rows.size()) >= kRowLimit) {
                truncated = true;
                break;
            }
            json row = json::array();
            for (int i = 0; i < columnCount; ++i) {
                const unsigned char* text = sqlite3_column_text(stmt, i);
                row.push_back(text ? json(reinterpret_cast<const char*>(text)) : json());
            }
            rows.push_back(std::move(row));
        }

        std::string stepError;
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {
            stepError = sqlite3_errmsg(db);
        }

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        if (!stepError.empty()) {
            return MakeErr("query_failed", stepError);
        }

        return MakeOk({{"columns", columns}, {"rows", rows}, {"truncated", truncated}});
    }

private:
    struct DbEntry {
        std::string id;
        std::string label;
        std::string path;
    };

    void LoadConfig(const std::string& configPath) {
        std::ifstream f(configPath);
        if (!f) return;  // 설정 파일 없으면 빈 목록 — db_config.json을 실제 경로로 채울 것
        json cfg;
        try {
            f >> cfg;
        } catch (...) {
            return;
        }
        for (const auto& item : cfg.value("databases", json::array())) {
            databases_.push_back(
                {item.value("id", ""), item.value("label", ""), item.value("path", "")});
        }
    }

    const DbEntry* Find(const std::string& id) const {
        for (const auto& db : databases_) {
            if (db.id == id) return &db;
        }
        return nullptr;
    }

    // table은 항상 sqlite_master 조회 결과에서만 온다 (사용자 입력 아님) — 문자열 결합 안전.
    static json ColumnsOf(sqlite3* db, const std::string& table) {
        json columns = json::array();
        sqlite3_stmt* stmt = nullptr;
        std::string sql = "PRAGMA table_info('" + table + "')";
        if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
                const unsigned char* typeRaw = sqlite3_column_text(stmt, 2);
                std::string type = typeRaw ? reinterpret_cast<const char*>(typeRaw) : "";
                columns.push_back({{"name", name}, {"type", type}});
            }
        }
        sqlite3_finalize(stmt);
        return columns;
    }

    std::vector<DbEntry> databases_;
};

std::unique_ptr<DbReader> CreateDbReader(const std::string& configPath) {
    return std::make_unique<DbReaderImpl>(configPath);
}

}  // namespace tester
