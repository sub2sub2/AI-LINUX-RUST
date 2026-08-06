const path = require('path');
const http = require('http');
const express = require('express');
const { WebSocketServer } = require('ws');

const { MockShim } = require('./mockShim');
const { ShimClient } = require('./shimClient');

const PORT = process.env.PORT || 8787;
const SHIM_MODE = process.env.SHIM_MODE || 'mock'; // 'mock' | 'socket'
const SHIM_SOCKET_PATH = process.env.SHIM_SOCKET_PATH || '/tmp/contextengine-tester-shim.sock';

async function createShim() {
  if (SHIM_MODE === 'socket') {
    const shim = new ShimClient(SHIM_SOCKET_PATH);
    await shim.connect();
    return shim;
  }
  const shim = new MockShim();
  shim.startDemoTicker();
  return shim;
}

async function main() {
  const shim = await createShim();

  const app = express();
  app.use(express.json());
  app.use(express.static(path.join(__dirname, '..', '..', 'web')));

  app.get('/api/context', async (req, res) => {
    const result = await shim.request('list-context', { category: req.query.category || '' });
    res.status(result.status === 'ok' ? 200 : 500).json(result);
  });

  app.get('/api/context/:category/:key', async (req, res) => {
    const result = await shim.request('get-context', {
      category: req.params.category,
      key: req.params.key,
    });
    res.status(result.status === 'ok' ? 200 : 404).json(result);
  });

  app.post('/api/context', async (req, res) => {
    const { category, key, value } = req.body || {};
    if (!category || !key) {
      res.status(400).json({ status: 'error', code: 'bad_request', message: 'category/key required' });
      return;
    }
    const result = await shim.request('set-context', { category, key, value });
    res.status(result.status === 'ok' ? 200 : 500).json(result);
  });

  app.delete('/api/context/:category/:key', async (req, res) => {
    const result = await shim.request('delete-context', {
      category: req.params.category,
      key: req.params.key,
    });
    res.status(result.status === 'ok' ? 200 : 500).json(result);
  });

  app.post('/api/context/clear-test', async (req, res) => {
    const result = await shim.request('clear-test-context', {});
    res.status(result.status === 'ok' ? 200 : 500).json(result);
  });

  const server = http.createServer(app);
  const wss = new WebSocketServer({ server, path: '/ws' });

  shim.on('event', (msg) => {
    const payload = JSON.stringify(msg);
    wss.clients.forEach((client) => {
      if (client.readyState === 1) client.send(payload);
    });
  });

  server.listen(PORT, () => {
    console.log(`[bridge-server] listening on http://localhost:${PORT} (SHIM_MODE=${SHIM_MODE})`);
  });
}

main().catch((err) => {
  console.error('[bridge-server] fatal:', err);
  process.exit(1);
});
