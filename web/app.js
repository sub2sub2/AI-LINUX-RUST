const tbody = document.getElementById('context-tbody');
const wsStatus = document.getElementById('ws-status');
const injectForm = document.getElementById('inject-form');
const clearTestBtn = document.getElementById('clear-test-btn');
const catalogTbody = document.getElementById('catalog-tbody');
const catalogRefreshBtn = document.getElementById('catalog-refresh-btn');

const rows = new Map(); // `${category}:${key}` -> entry

function rowKey(category, key) {
  return `${category}:${key}`;
}

function formatValue(value) {
  if (value === undefined) return '';
  return typeof value === 'string' ? value : JSON.stringify(value);
}

function formatTime(ts) {
  if (!ts) return '';
  return new Date(ts).toLocaleTimeString();
}

function render() {
  const sorted = [...rows.values()].sort((a, b) =>
    (a.category + a.key).localeCompare(b.category + b.key)
  );

  tbody.innerHTML = '';
  for (const entry of sorted) {
    const tr = document.createElement('tr');
    tr.dataset.key = rowKey(entry.category, entry.key);
    tr.innerHTML = `
      <td>${entry.category}</td>
      <td>${entry.key}</td>
      <td><code>${escapeHtml(formatValue(entry.value))}</code></td>
      <td>${entry.source || ''}</td>
      <td>${formatTime(entry.timestamp)}</td>
      <td><button class="icon-btn" data-action="delete">삭제</button></td>
    `;
    tr.querySelector('[data-action="delete"]').addEventListener('click', () =>
      deleteContext(entry.category, entry.key)
    );
    tbody.appendChild(tr);
  }
}

function flashRow(category, key) {
  const tr = tbody.querySelector(`tr[data-key="${CSS.escape(rowKey(category, key))}"]`);
  if (!tr) return;
  tr.classList.remove('flash');
  // eslint-disable-next-line no-unused-expressions
  tr.offsetWidth; // reflow to restart animation
  tr.classList.add('flash');
}

function escapeHtml(str) {
  const div = document.createElement('div');
  div.textContent = str;
  return div.innerHTML;
}

async function loadCatalog() {
  const res = await fetch('/api/catalog');
  const body = await res.json();
  if (body.status !== 'ok') return;
  renderCatalog(body.data.items || []);
}

function renderCatalog(items) {
  const sorted = [...items].sort((a, b) => (a.plugin_id + a.key).localeCompare(b.plugin_id + b.key));
  catalogTbody.innerHTML = '';
  for (const entry of sorted) {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td>${entry.plugin_id}</td>
      <td>${entry.category}</td>
      <td>${entry.key}</td>
      <td>${entry.data_type || ''}</td>
      <td>${entry.description || ''}</td>
      <td><span class="badge badge-${entry.status}">${entry.status}</span></td>
      <td>${entry.lastReportedAt ? formatTime(entry.lastReportedAt) : '-'}</td>
      <td><code>${escapeHtml(entry.lastValue !== null && entry.lastValue !== undefined ? formatValue(entry.lastValue) : '')}</code></td>
    `;
    catalogTbody.appendChild(tr);
  }
}

async function loadInitial() {
  const res = await fetch('/api/context');
  const body = await res.json();
  if (body.status !== 'ok') return;
  for (const item of body.data.items || []) {
    rows.set(rowKey(item.category, item.key), item);
  }
  render();
}

async function injectContext(category, key, rawValue) {
  let value;
  try {
    value = JSON.parse(rawValue);
  } catch {
    value = rawValue; // JSON이 아니면 문자열 그대로
  }
  await fetch('/api/context', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ category, key, value }),
  });
}

async function deleteContext(category, key) {
  await fetch(`/api/context/${encodeURIComponent(category)}/${encodeURIComponent(key)}`, {
    method: 'DELETE',
  });
}

async function clearTestContext() {
  await fetch('/api/context/clear-test', { method: 'POST' });
}

function connectWs() {
  const protocol = location.protocol === 'https:' ? 'wss' : 'ws';
  const ws = new WebSocket(`${protocol}://${location.host}/ws`);

  ws.addEventListener('open', () => setStatus('connected'));
  ws.addEventListener('close', () => {
    setStatus('disconnected');
    setTimeout(connectWs, 2000); // 재연결 시도
  });
  ws.addEventListener('error', () => ws.close());

  ws.addEventListener('message', (ev) => {
    const msg = JSON.parse(ev.data);
    if (msg.event === 'context_changed') {
      rows.set(rowKey(msg.category, msg.key), {
        category: msg.category,
        key: msg.key,
        value: msg.value,
        source: msg.source,
        timestamp: msg.timestamp,
      });
      render();
      flashRow(msg.category, msg.key);
    } else if (msg.event === 'context_deleted') {
      rows.delete(rowKey(msg.category, msg.key));
      render();
    }
  });
}

function setStatus(state) {
  wsStatus.className = `status status-${state}`;
  wsStatus.textContent = state === 'connected' ? 'connected' : state === 'disconnected' ? 'disconnected' : 'connecting…';
}

injectForm.addEventListener('submit', async (ev) => {
  ev.preventDefault();
  const formData = new FormData(injectForm);
  await injectContext(formData.get('category'), formData.get('key'), formData.get('value'));
  injectForm.reset();
});

clearTestBtn.addEventListener('click', clearTestContext);
catalogRefreshBtn.addEventListener('click', loadCatalog);

loadInitial();
loadCatalog();
connectWs();
setInterval(loadCatalog, 5000);
