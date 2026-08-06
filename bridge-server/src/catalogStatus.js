// catalog(정의) 항목과 context(실측) 항목을 category+key로 대조해서 로드 상태를 계산한다.
// docs/interface.md 2.4절 규칙: stale 판정은 report_interval_ms가 있을 때만 적용.

const STALE_MULTIPLIER = 3;

function computeCatalogStatus(catalogItems, contextItems, now = Date.now()) {
  const contextIndex = new Map();
  for (const item of contextItems) {
    contextIndex.set(`${item.category}:${item.key}`, item);
  }

  return catalogItems.map((entry) => {
    const ctx = contextIndex.get(`${entry.category}:${entry.key}`);

    let status;
    if (!ctx) {
      status = 'missing';
    } else if (entry.report_interval_ms && now - ctx.timestamp > entry.report_interval_ms * STALE_MULTIPLIER) {
      status = 'stale';
    } else {
      status = 'ok';
    }

    return {
      ...entry,
      status,
      lastReportedAt: ctx ? ctx.timestamp : null,
      lastValue: ctx ? ctx.value : null,
      lastSource: ctx ? ctx.source : null,
    };
  });
}

module.exports = { computeCatalogStatus };
