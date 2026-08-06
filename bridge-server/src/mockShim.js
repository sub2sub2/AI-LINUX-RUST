// native-shim(실제 디바이스) 없이 bridge-server/web을 개발·테스트하기 위한 인메모리 mock.
// ShimClient와 동일한 인터페이스(request(method, params), 'event' 이벤트)를 제공한다.

const { EventEmitter } = require('events');
const { MockDb } = require('./mockDb');

class MockShim extends EventEmitter {
  constructor() {
    super();
    this.store = new Map(); // `${category}:${key}` -> { category, key, value, source, timestamp }
    this.mockDb = new MockDb();

    // list-catalog 목데이터. 실제 스키마는 확인되지 않아 가정치임 (docs/interface.md 2.1 참고).
    // 일부러 일부 항목엔 대응하는 context가 없도록/오래되도록 구성해서 missing/stale 데모.
    this.catalog = [
      {
        plugin_id: 'battery-plugin',
        category: 'Device',
        key: 'battery',
        data_type: 'object',
        description: '배터리 상태',
        report_interval_ms: 5000,
      },
      {
        plugin_id: 'location-plugin',
        category: 'Device',
        key: 'location',
        data_type: 'object',
        description: '위치 정보',
        report_interval_ms: 10000,
      },
      {
        plugin_id: 'system-plugin',
        category: 'System',
        key: 'uptime',
        data_type: 'number',
        description: '부팅 후 경과 시간',
        report_interval_ms: 2000,
      },
    ];

    // system-plugin은 한 번만 보고하고 멈춘 상황을 흉내냄 (시간이 지나면 stale이 됨).
    this.store.set(this._k('System', 'uptime'), {
      category: 'System',
      key: 'uptime',
      value: 12,
      source: 'system-plugin',
      timestamp: Date.now(),
    });
  }

  connect() {
    return Promise.resolve();
  }

  async request(method, params = {}) {
    switch (method) {
      case 'get-context': {
        const entry = this.store.get(this._k(params.category, params.key));
        return entry ? this._ok(entry) : this._err('not_found', 'no such context');
      }
      case 'set-context': {
        const entry = {
          category: params.category,
          key: params.key,
          value: params.value,
          source: params.source || 'test-injected',
          timestamp: Date.now(),
        };
        this.store.set(this._k(params.category, params.key), entry);
        this._emitChange(entry, false);
        return this._ok(entry);
      }
      case 'delete-context': {
        const k = this._k(params.category, params.key);
        const existed = this.store.delete(k);
        if (existed) this._emitChange({ category: params.category, key: params.key }, true);
        return this._ok({ deleted: existed });
      }
      case 'list-context': {
        const items = [...this.store.values()].filter(
          (e) => !params.category || e.category === params.category
        );
        return this._ok({ items });
      }
      case 'list-catalog': {
        const items = this.catalog.filter(
          (e) => !params.plugin_id || e.plugin_id === params.plugin_id
        );
        return this._ok({ items });
      }
      case 'db-list': {
        return this._ok({ databases: this.mockDb.list() });
      }
      case 'db-schema': {
        const tables = this.mockDb.schema(params.db_id);
        if (!tables) return this._err('db_not_found', `unknown db id: ${params.db_id}`);
        return this._ok({ tables });
      }
      case 'db-query': {
        const result = this.mockDb.query(params.db_id, params.sql);
        if (!result.ok) return this._err(result.code, result.message);
        return this._ok({
          columns: result.columns,
          rows: result.rows,
          truncated: result.truncated,
        });
      }
      case 'clear-test-context': {
        let count = 0;
        for (const [k, e] of this.store) {
          if (e.source === 'test-injected') {
            this.store.delete(k);
            count += 1;
            this._emitChange({ category: e.category, key: e.key }, true);
          }
        }
        return this._ok({ cleared: count });
      }
      default:
        return this._err('unknown_method', `unsupported method: ${method}`);
    }
  }

  // 실제 plugin이 주기적으로 보고하는 상황을 흉내내는 데모용 티커 (live view 검증용).
  startDemoTicker(intervalMs = 4000) {
    let n = 0;
    this._demoTimer = setInterval(() => {
      n += 1;
      const entry = {
        category: 'Device',
        key: 'battery',
        value: { level: 100 - (n % 60), charging: n % 2 === 0 },
        source: 'demo-plugin',
        timestamp: Date.now(),
      };
      this.store.set(this._k(entry.category, entry.key), entry);
      this._emitChange(entry, false);
    }, intervalMs);
  }

  stopDemoTicker() {
    clearInterval(this._demoTimer);
  }

  _k(category, key) {
    return `${category}:${key}`;
  }

  _ok(data) {
    return { status: 'ok', data };
  }

  _err(code, message) {
    return { status: 'error', code, message };
  }

  _emitChange(entry, deleted) {
    this.emit('event', {
      type: 'event',
      event: deleted ? 'context_deleted' : 'context_changed',
      category: entry.category,
      key: entry.key,
      value: deleted ? undefined : entry.value,
      timestamp: entry.timestamp || Date.now(),
      source: entry.source,
    });
  }
}

module.exports = { MockShim };
