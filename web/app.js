const tbody = document.getElementById('context-tbody');
const wsStatus = document.getElementById('ws-status');
const injectForm = document.getElementById('inject-form');
const clearTestBtn = document.getElementById('clear-test-btn');
const catalogTbody = document.getElementById('catalog-tbody');
const catalogRefreshBtn = document.getElementById('catalog-refresh-btn');
const dbSelect = document.getElementById('db-select');
const tableSelect = document.getElementById('table-select');
const dbSearchInput = document.getElementById('db-search-input');
const queryForm = document.getElementById('query-form');
const queryInput = document.getElementById('query-input');
const queryError = document.getElementById('query-error');
const queryResultHead = document.getElementById('query-result-head');
const queryResultBody = document.getElementById('query-result-body');
const queryRowCountHint = document.getElementById('query-row-count-hint');

const BROWSE_LIMIT = 500;
let dbSchemas = {}; // dbId -> tables
let currentResult = { columns: [], rows: [], truncated: false }; // 검색/필터의 원본 데이터

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

async function loadDbList() {
  const res = await fetch('/api/db');
  const body = await res.json();
  if (body.status !== 'ok') return;

  const databases = body.data.databases || [];
  dbSelect.innerHTML = databases
    .map((db) => `<option value="${escapeHtml(db.id)}">${escapeHtml(db.label)}</option>`)
    .join('');

  if (databases.length > 0) {
    await loadDbSchema(databases[0].id);
  }
}

async function loadDbSchema(dbId) {
  const res = await fetch(`/api/db/${encodeURIComponent(dbId)}/schema`);
  const body = await res.json();
  if (body.status !== 'ok') {
    tableSelect.innerHTML = '';
    return;
  }

  const tables = body.data.tables || [];
  dbSchemas[dbId] = tables;
  tableSelect.innerHTML = tables
    .map((t) => `<option value="${escapeHtml(t.name)}">${escapeHtml(t.name)}</option>`)
    .join('');

  if (tables.length > 0) {
    await browseTable(dbId, tables[0].name);
  }
}

// SQL을 몰라도 되도록: 테이블을 고르면 전체 데이터를 가져오고, 이후 검색은 클라이언트에서
// 필터링한다 (별도 쿼리 문법 입력 불필요).
async function browseTable(dbId, tableName) {
  dbSearchInput.value = '';
  await runQuery(dbId, `SELECT * FROM ${tableName} LIMIT ${BROWSE_LIMIT}`);
}

async function runQuery(dbId, sql) {
  queryError.hidden = true;

  const res = await fetch(`/api/db/${encodeURIComponent(dbId)}/query`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ sql }),
  });
  const body = await res.json();

  if (body.status !== 'ok') {
    currentResult = { columns: [], rows: [], truncated: false };
    renderResultRows([]);
    queryError.textContent = `[${body.code || 'error'}] ${body.message || '조회 실패'}`;
    queryError.hidden = false;
    return;
  }

  currentResult = {
    columns: body.data.columns,
    rows: body.data.rows,
    truncated: body.data.truncated,
  };
  applySearchFilter();
}

function applySearchFilter() {
  const term = dbSearchInput.value.trim().toLowerCase();
  const filtered = term
    ? currentResult.rows.filter((row) =>
        row.some((v) => v !== null && String(v).toLowerCase().includes(term))
      )
    : currentResult.rows;

  queryResultHead.innerHTML = currentResult.columns.map((c) => `<th>${escapeHtml(c)}</th>`).join('');
  renderResultRows(filtered);

  const parts = [`${filtered.length}행`];
  if (term) parts.push(`(전체 ${currentResult.rows.length}행 중 검색됨)`);
  if (currentResult.truncated) parts.push('— DB에 더 많은 행이 있어 일부만 불러왔습니다');
  queryRowCountHint.textContent = parts.join(' ');
}

function renderResultRows(rows) {
  queryResultBody.innerHTML = rows
    .map(
      (row) =>
        `<tr>${row.map((v) => `<td>${escapeHtml(v === null ? 'NULL' : String(v))}</td>`).join('')}</tr>`
    )
    .join('');
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
dbSelect.addEventListener('change', () => loadDbSchema(dbSelect.value));
tableSelect.addEventListener('change', () => browseTable(dbSelect.value, tableSelect.value));
dbSearchInput.addEventListener('input', applySearchFilter);
queryForm.addEventListener('submit', (ev) => {
  ev.preventDefault();
  const sql = queryInput.value.trim();
  if (!sql || !dbSelect.value) return;
  runQuery(dbSelect.value, sql);
});

loadInitial();
loadCatalog();
loadDbList();
connectWs();
setInterval(loadCatalog, 5000);
