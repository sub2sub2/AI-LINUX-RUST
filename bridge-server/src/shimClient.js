// native-shim(로컬 유닉스 소켓, docs/interface.md 3절 프로토콜)에 연결하는 실제 클라이언트.
// 디바이스/에뮬레이터 위에서 native-shim이 실행 중일 때 SHIM_MODE=socket 으로 사용한다.

const net = require('net');
const crypto = require('crypto');
const { EventEmitter } = require('events');

class ShimClient extends EventEmitter {
  constructor(socketPath) {
    super();
    this.socketPath = socketPath;
    this.socket = null;
    this.buffer = '';
    this.pending = new Map();
  }

  connect() {
    this.socket = net.createConnection(this.socketPath);
    this.socket.on('data', (chunk) => this._onData(chunk));
    this.socket.on('error', (err) => this.emit('error', err));
    this.socket.on('close', () => this.emit('close'));
    return new Promise((resolve, reject) => {
      this.socket.once('connect', resolve);
      this.socket.once('error', reject);
    });
  }

  _onData(chunk) {
    this.buffer += chunk.toString('utf8');
    let idx;
    while ((idx = this.buffer.indexOf('\n')) >= 0) {
      const line = this.buffer.slice(0, idx);
      this.buffer = this.buffer.slice(idx + 1);
      if (!line.trim()) continue;

      let msg;
      try {
        msg = JSON.parse(line);
      } catch {
        continue;
      }

      if (msg.type === 'response' && this.pending.has(msg.id)) {
        const { resolve } = this.pending.get(msg.id);
        this.pending.delete(msg.id);
        resolve(msg);
      } else if (msg.type === 'event') {
        this.emit('event', msg);
      }
    }
  }

  request(method, params = {}) {
    const id = crypto.randomUUID();
    const payload = JSON.stringify({ type: 'request', id, method, params }) + '\n';
    return new Promise((resolve, reject) => {
      this.pending.set(id, { resolve, reject });
      this.socket.write(payload, (err) => {
        if (err) {
          this.pending.delete(id);
          reject(err);
        }
      });
    });
  }
}

module.exports = { ShimClient };
