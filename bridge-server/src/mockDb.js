// native-shim의 db_reader.cpp를 대신하는 mock: 실제 SQLite(Node 22 내장 node:sqlite)로
// 임시 DB 파일을 만들어 db-list/db-schema/db-query를 진짜로 실행해본다.
// 쓰기 차단은 DatabaseSync(path, { readOnly: true })로 검증 — native-shim의
// SQLITE_OPEN_READONLY와 동일한 방어선.

const { DatabaseSync } = require('node:sqlite');
const path = require('path');
const os = require('os');
const crypto = require('crypto');

const DB_ID = 'mock_db';

class MockDb {
  constructor() {
    this.filePath = path.join(os.tmpdir(), `contextengine-tester-mock-${crypto.randomUUID()}.db`);

    const db = new DatabaseSync(this.filePath);
    db.exec(`
      CREATE TABLE plugins (
        id INTEGER PRIMARY KEY,
        plugin_id TEXT,
        status TEXT,
        loaded_at INTEGER
      );
      CREATE TABLE context_log (
        id INTEGER PRIMARY KEY,
        category TEXT,
        key TEXT,
        value TEXT,
        ts INTEGER
      );
    `);
    const insertPlugin = db.prepare(
      'INSERT INTO plugins (plugin_id, status, loaded_at) VALUES (?, ?, ?)'
    );
    insertPlugin.run('battery-plugin', 'running', Date.now());
    insertPlugin.run('location-plugin', 'stopped', Date.now());
    insertPlugin.run('system-plugin', 'running', Date.now());

    const insertLog = db.prepare(
      'INSERT INTO context_log (category, key, value, ts) VALUES (?, ?, ?, ?)'
    );
    insertLog.run('Device', 'battery', '{"level":95,"charging":false}', Date.now());
    insertLog.run('System', 'uptime', '12', Date.now());

    db.close();
  }

  list() {
    return [{ id: DB_ID, label: 'Mock DB (bridge-server 데모용 임시 SQLite)', path: this.filePath }];
  }

  schema(dbId) {
    if (dbId !== DB_ID) return null;
    const db = new DatabaseSync(this.filePath, { readOnly: true });
    try {
      const tables = db.prepare("SELECT name FROM sqlite_master WHERE type='table'").all();
      return tables.map((t) => {
        const cols = db.prepare(`PRAGMA table_info(${t.name})`).all();
        return { name: t.name, columns: cols.map((c) => ({ name: c.name, type: c.type })) };
      });
    } finally {
      db.close();
    }
  }

  query(dbId, sql) {
    if (dbId !== DB_ID) {
      return { ok: false, code: 'db_not_found', message: `unknown db id: ${dbId}` };
    }

    // 실제 native-shim의 SQLITE_OPEN_READONLY와 동일하게, 연결 자체를 읽기 전용으로 연다.
    // node:sqlite에는 statement 단위 readonly 검사 API가 없어 연결 레벨 차단에 의존한다
    // (C++ 쪽은 sqlite3_stmt_readonly()로 한 단계 더 검증 — docs/interface.md 2.5절 참고).
    const db = new DatabaseSync(this.filePath, { readOnly: true });
    try {
      const stmt = db.prepare(sql);
      const columnMeta = stmt.columns();
      const columns = columnMeta.map((c) => c.name);
      const rows = stmt.all().map((row) => columns.map((c) => row[c]));
      return { ok: true, columns, rows, truncated: false };
    } catch (err) {
      return { ok: false, code: 'query_failed', message: err.message };
    } finally {
      db.close();
    }
  }
}

module.exports = { MockDb };
