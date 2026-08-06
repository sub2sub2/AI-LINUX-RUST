// native-shim(실제 디바이스) 없이 bridge-server/web을 개발·테스트하기 위한 인메모리 mock.
// ShimClient와 동일한 인터페이스(request(method, params), 'event' 이벤트)를 제공한다.

const { EventEmitter } = require('events');

class MockShim extends EventEmitter {
  constructor() {
    super();
    this.store = new Map(); // `${category}:${key}` -> { category, key, value, source, timestamp }
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
