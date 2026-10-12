#!/usr/bin/env python3
"""Summary of build/report.json, the objdiff-cli progress report.

    tools/progress.py [report.json] [--write] [--html PATH]

Prints the overall totals and one row per category: matched code bytes,
functions, named functions, data and complete units. Percentages come from
the byte and function counts. A function is named once its symbol is no
longer splat's func_XXXXXXXX; objdiff has no such measure, so it is counted
here from the report's function names.

--write refreshes docs/progress/, which is committed: summary.json (no
per-function data), badge-*.json (shields.io endpoints), index.html, a
static page with the summary and a unit map embedded, and report.json, the
objdiff report itself (CI uploads it for decomp.dev). It also replaces the
table between the progress markers in README.md.

--html PATH writes the same page with each unit's functions expandable. The
page holds symbol names and sizes of the local build: keep it under build/.
"""
import argparse
import datetime
import json
import re
import sys
from html import escape
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
DOCS = ROOT / "docs" / "progress"
README = ROOT / "README.md"
START, END = "<!-- progress:start -->", "<!-- progress:end -->"
AUTO_NAME = re.compile(r"(func|sub|D)_[0-9A-Fa-f]{8}$")


def measures_of(m: dict) -> dict:
    # proto3-JSON omits zero fields and writes 64-bit counts as strings
    return {k: round(float(v), 4) if k.endswith("percent") else int(v)
            for k, v in m.items()}


def pct(part, whole) -> float:
    return 100.0 * part / whole if whole else 0.0


def num(n) -> str:
    return f"{n:,}"


def code_pct(m: dict) -> float:
    return pct(m.get("matched_code", 0), m.get("total_code", 0))


def named_pct(m: dict) -> float:
    return pct(m.get("named_functions", 0), m.get("total_functions", 0))


def is_named(name: str) -> bool:
    return not AUTO_NAME.match(name)


def unit_entry(u: dict, detail: bool) -> dict:
    m = measures_of(u.get("measures", {}))
    meta = u.get("metadata", {})
    named = [f for f in u.get("functions", []) if is_named(f["name"])]
    e = {
        "name": u["name"],
        "category": (meta.get("progress_categories") or ["none"])[0],
        "code_total": m.get("total_code", 0),
        "code_matched": m.get("matched_code", 0),
        "fuzzy": round(m.get("fuzzy_match_percent", 0.0), 2),
        "functions_total": m.get("total_functions", 0),
        "functions_matched": m.get("matched_functions", 0),
        "functions_named": len(named),
        "code_named": sum(int(f.get("size", 0)) for f in named),
        "data_total": m.get("total_data", 0),
        "data_matched": m.get("matched_data", 0),
        "complete": bool(meta.get("complete", False)),
    }
    if detail:
        e["functions"] = [{"name": f["name"], "size": int(f.get("size", 0)),
                           "fuzzy": round(float(f.get("fuzzy_match_percent", 0)), 2)}
                          for f in u.get("functions", [])]
    return e


def add_named(measures: dict, units: list):
    measures["named_functions"] = sum(u["functions_named"] for u in units)
    measures["named_code"] = sum(u["code_named"] for u in units)


def load(path: Path, detail: bool) -> dict:
    r = json.loads(path.read_text())
    data = {
        "generated": datetime.datetime.now(datetime.timezone.utc).date().isoformat(),
        "measures": measures_of(r.get("measures", {})),
        "categories": [{"id": c["id"], "name": c.get("name", c["id"]),
                        "measures": measures_of(c.get("measures", {}))}
                       for c in r.get("categories", [])],
        "units": [unit_entry(u, detail) for u in r.get("units", [])],
    }
    add_named(data["measures"], data["units"])
    for c in data["categories"]:
        add_named(c["measures"], [u for u in data["units"] if u["category"] == c["id"]])
    return data


def rows(data) -> list:
    return [("overall", data["measures"])] + [(c["id"], c["measures"])
                                              for c in data["categories"]]


def terminal(data) -> str:
    m = data["measures"]
    out = [f"KFIV progress, {data['generated']}: {m.get('total_units', 0)} units",
           f"{'':8} {'code':>7}  {'code bytes':>23}  {'functions':>11}  "
           f"{'named':>11}  {'data':>7}  {'complete':>9}"]
    for label, x in rows(data):
        mc, tc = x.get("matched_code", 0), x.get("total_code", 0)
        mf, tf = x.get("matched_functions", 0), x.get("total_functions", 0)
        nf = x.get("named_functions", 0)
        dp = pct(x.get("matched_data", 0), x.get("total_data", 0))
        cu, tu = x.get("complete_units", 0), x.get("total_units", 0)
        out.append(f"{label:8} {code_pct(x):6.2f}%  {num(mc) + ' / ' + num(tc):>23}  "
                   f"{f'{mf}/{tf}':>11}  {f'{nf}/{tf}':>11}  {dp:6.2f}%  {f'{cu}/{tu}':>9}")
    return "\n".join(out)


def badge(label: str, value: float) -> dict:
    r = round(value, 1)
    color = ("red" if r < 25 else "orange" if r < 50 else "yellow" if r < 75
             else "yellowgreen" if r < 100 else "brightgreen")
    return {"schemaVersion": 1, "label": label, "message": f"{r:.1f}%", "color": color}


def badges(data) -> dict:
    m = data["measures"]
    cats = {c["id"]: c["measures"] for c in data["categories"]}
    return {
        "badge-code.json": badge("code", code_pct(m)),
        "badge-functions.json": badge("functions", pct(m.get("matched_functions", 0),
                                                       m.get("total_functions", 0))),
        "badge-sce.json": badge("sce", code_pct(cats.get("sce", {}))),
        "badge-game.json": badge("game", code_pct(cats.get("game", {}))),
        "badge-named.json": badge("game named", named_pct(cats.get("game", {}))),
    }


def readme_table(data) -> str:
    out = ["| | code | functions | named | units complete |",
           "| --- | ---: | ---: | ---: | ---: |"]
    for label, x in rows(data):
        mf, tf = x.get("matched_functions", 0), x.get("total_functions", 0)
        nf = x.get("named_functions", 0)
        cu, tu = x.get("complete_units", 0), x.get("total_units", 0)
        out.append(f"| {label} | {code_pct(x):.2f}% | {mf} / {tf} | {nf} / {tf} | {cu} / {tu} |")
    return "\n".join(out)


def cards_and_bars(data) -> tuple:
    m = data["measures"]
    mc, tc = m.get("matched_code", 0), m.get("total_code", 0)
    mf, tf = m.get("matched_functions", 0), m.get("total_functions", 0)
    md, td = m.get("matched_data", 0), m.get("total_data", 0)
    cu, tu = m.get("complete_units", 0), m.get("total_units", 0)
    cards = [
        ("Code matched", f"{code_pct(m):.2f}%", f"{num(mc)} of {num(tc)} bytes"),
        ("Functions matched", f"{pct(mf, tf):.2f}%", f"{mf} of {tf}"),
        ("Functions named", f"{named_pct(m):.2f}%",
         f"{m.get('named_functions', 0)} of {tf}"),
        ("Data matched", f"{pct(md, td):.2f}%", f"{num(md)} of {num(td)} bytes"),
        ("Units complete", f"{cu} of {tu}", f"{pct(cu, tu):.2f}% of units"),
    ]
    html_cards = "".join(
        f'<div class="card"><span>{escape(a)}</span><b>{escape(b)}</b>'
        f'<span>{escape(c)}</span></div>' for a, b, c in cards)
    bars = []
    for c in data["categories"]:
        x = c["measures"]
        p = code_pct(x)
        mf, tf = x.get("matched_functions", 0), x.get("total_functions", 0)
        nf = x.get("named_functions", 0)
        dp = pct(x.get("matched_data", 0), x.get("total_data", 0))
        cu, tu = x.get("complete_units", 0), x.get("total_units", 0)
        bars.append(
            f'<section class="cat"><h2>{escape(c["name"])} '
            f'<span class="muted">({escape(c["id"])})</span></h2>'
            f'<div class="bar" role="img" aria-label="{p:.2f} percent of code matched">'
            f'<i style="width:{min(max(p, 0.0), 100.0):.2f}%"></i></div>'
            f'<p class="muted">code {p:.2f}% ({num(x.get("matched_code", 0))} of '
            f'{num(x.get("total_code", 0))} bytes), functions {mf}/{tf}, named {nf}/{tf}, '
            f'data {dp:.2f}%, '
            f'units {cu}/{tu} complete</p></section>')
    return html_cards, "".join(bars)


PAGE = """<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>KFIV decompilation progress</title>
<style>
:root {
  --bg: #fbfbfc; --fg: #1c1e21; --muted: #5f6570; --line: #d8dbe0;
  --track: #e4e6ea; --bar: #2f7d4f; --accent: #2f5fb3; --row: #f0f2f5;
  --map-done: #2f7d4f; --map-part: #c27c0e; --map-none: #d3d7dd;
}
@media (prefers-color-scheme: dark) {
  :root:not([data-theme="light"]) {
    --bg: #16181c; --fg: #e6e8eb; --muted: #9aa1ab; --line: #353a42;
    --track: #2b3038; --bar: #4caf72; --accent: #7aa6ec; --row: #1e2227;
    --map-done: #3f9a62; --map-part: #b97a1e; --map-none: #323842;
  }
}
:root[data-theme="dark"] {
  --bg: #16181c; --fg: #e6e8eb; --muted: #9aa1ab; --line: #353a42;
  --track: #2b3038; --bar: #4caf72; --accent: #7aa6ec; --row: #1e2227;
  --map-done: #3f9a62; --map-part: #b97a1e; --map-none: #323842;
}
body { margin: 0; background: var(--bg); color: var(--fg);
  font: 15px/1.45 system-ui, -apple-system, "Segoe UI", sans-serif; }
main { max-width: 1000px; margin: 0 auto; padding: 16px; }
h1 { font-size: 1.5rem; margin: 8px 0 4px; }
h2 { font-size: 1.05rem; margin: 18px 0 6px; }
.muted { color: var(--muted); font-size: .9rem; }
.cards { display: grid; grid-template-columns: repeat(auto-fit, minmax(160px, 1fr));
  gap: 10px; margin: 16px 0; }
.card { border: 1px solid var(--line); border-radius: 6px; padding: 10px 12px; }
.card b { display: block; font-size: 1.25rem; margin: 2px 0; }
.card span { color: var(--muted); font-size: .85rem; }
.bar { height: 14px; background: var(--track); border-radius: 4px; overflow: hidden; }
.bar i { display: block; height: 100%; background: var(--bar); }
.cat p { margin: 4px 0 0; }
.controls { display: flex; flex-wrap: wrap; gap: 8px; align-items: center; margin: 18px 0 8px; }
input, select { font: inherit; padding: 4px 6px; color: var(--fg); background: var(--bg);
  border: 1px solid var(--line); border-radius: 4px; }
.wrap { overflow-x: auto; }
table { border-collapse: collapse; width: 100%; }
th, td { text-align: left; padding: 5px 8px; border-bottom: 1px solid var(--line);
  vertical-align: top; }
th { cursor: pointer; user-select: none; white-space: nowrap; }
td.n, th.n { text-align: right; font-variant-numeric: tabular-nums; white-space: nowrap; }
.mono { font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; font-size: .9rem;
  word-break: break-all; }
button.link { all: unset; cursor: pointer; color: var(--accent);
  font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; font-size: .9rem; }
button.link:focus-visible { outline: 2px solid var(--accent); }
tr.detail td { background: var(--row); }
table.fns th { cursor: default; }
footer { margin: 24px 0 8px; }
.legend { display: flex; flex-wrap: wrap; gap: 4px 16px; margin: 4px 0 8px; font-size: .9rem; }
.legend span::before { content: ""; display: inline-block; width: 10px; height: 10px;
  margin-right: 6px; border-radius: 2px; background: var(--sw); vertical-align: -1px; }
#map { position: relative; height: clamp(320px, 60vw, 600px); }
#map canvas { display: block; width: 100%; height: 100%; cursor: pointer; }
#tip { position: absolute; pointer-events: none; z-index: 1; max-width: 280px;
  padding: 6px 8px; border: 1px solid var(--line); border-radius: 6px;
  background: var(--bg); box-shadow: 0 2px 8px rgba(0,0,0,.18); font-size: .85rem; }
#tip b { display: block; font-weight: 600; word-break: break-all;
  font-family: ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; }
</style>
</head>
<body>
<main>
<h1>KFIV decompilation progress</h1>
<p class="muted">Generated @@GEN@@ from objdiff-cli's report against the retail ELF.</p>
<div class="cards">@@CARDS@@</div>
@@BARS@@
<h2>Map</h2>
<p class="muted">One cell per unit, sized by its code. Hover for details, click to find it
in the table. The filters below apply to the map too.</p>
<div class="legend" id="legend"></div>
<div id="map"><canvas id="mapcv" role="img"></canvas><div id="tip" hidden></div></div>
<div class="controls">
  <input id="q" type="search" placeholder="Filter units" aria-label="Filter units">
  <select id="cat" aria-label="Category"><option value="all">All categories</option>@@CATOPTS@@</select>
  <span class="muted" id="count"></span>
</div>
<div class="wrap">
<table id="units">
<thead><tr>
<th data-k="name">Unit</th><th data-k="category">Category</th>
<th class="n" data-k="code_total">Size</th><th class="n" data-k="cp">Code</th><th class="n" data-k="fp">Functions</th>
<th class="n" data-k="np">Named</th>
<th class="n" data-k="dp">Data</th><th data-k="complete">Complete</th>
</tr></thead>
<tbody></tbody>
</table>
</div>
<footer class="muted">Size is code bytes. Code and data are matched bytes over total
bytes. Named counts functions no longer called func_XXXXXXXX. Click a column header to sort. Units are in link order by default.</footer>
</main>
<script id="data" type="application/json">@@DATA@@</script>
<script>
(function () {
  "use strict";
  var D = JSON.parse(document.getElementById("data").textContent);
  var tbody = document.querySelector("#units tbody");
  var q = document.getElementById("q");
  var cat = document.getElementById("cat");
  var count = document.getElementById("count");
  var key = null, dir = 1, opened = {};

  function pct(a, b) { return b ? 100 * a / b : null; }
  function txt(p) { return p === null ? "-" : p.toFixed(2) + "%"; }
  function num(n) { return n.toLocaleString("en-US"); }
  function cell(tag, cls, text) {
    var td = document.createElement(tag);
    if (cls) td.className = cls;
    td.textContent = text;
    return td;
  }

  var units = D.units.map(function (u, i) {
    u.i = i;
    u.cp = pct(u.code_matched, u.code_total);
    u.fp = pct(u.functions_matched, u.functions_total);
    u.np = pct(u.functions_named, u.functions_total);
    u.dp = pct(u.data_matched, u.data_total);
    return u;
  });

  function fnRows(u) {
    var tr = document.createElement("tr");
    tr.className = "detail";
    var td = document.createElement("td");
    td.colSpan = 8;
    var t = document.createElement("table");
    t.className = "fns";
    var head = document.createElement("tr");
    head.appendChild(cell("th", "", "Function (address order)"));
    head.appendChild(cell("th", "n", "Size"));
    head.appendChild(cell("th", "n", "Fuzzy"));
    t.appendChild(head);
    u.functions.forEach(function (f) {
      var r = document.createElement("tr");
      r.appendChild(cell("td", "mono", f.name));
      r.appendChild(cell("td", "n", num(f.size)));
      r.appendChild(cell("td", "n", f.fuzzy.toFixed(2) + "%"));
      t.appendChild(r);
    });
    td.appendChild(t);
    tr.appendChild(td);
    return tr;
  }

  function render() {
    var needle = q.value.trim().toLowerCase(), c = cat.value;
    var list = units.filter(function (u) {
      return (!needle || u.name.toLowerCase().indexOf(needle) >= 0) &&
             (c === "all" || u.category === c);
    });
    if (key) {
      list.sort(function (a, b) {
        var x = a[key], y = b[key];
        if (x === y) return a.i - b.i;
        if (x === null) return 1;
        if (y === null) return -1;
        return (x < y ? -1 : 1) * dir;
      });
    }
    tbody.textContent = "";
    list.forEach(function (u) {
      var tr = document.createElement("tr");
      var name = cell("td", "mono", "");
      if (u.functions && u.functions.length) {
        var b = document.createElement("button");
        b.type = "button";
        b.className = "link";
        b.textContent = u.name;
        b.setAttribute("aria-expanded", opened[u.i] ? "true" : "false");
        b.addEventListener("click", function () {
          opened[u.i] = !opened[u.i];
          render();
        });
        name.appendChild(b);
      } else {
        name.textContent = u.name;
      }
      tr.appendChild(name);
      tr.appendChild(cell("td", "", u.category));
      tr.appendChild(cell("td", "n", u.code_total ? num(u.code_total) : "-"));
      tr.appendChild(cell("td", "n", txt(u.cp)));
      tr.appendChild(cell("td", "n", u.functions_total ?
        u.functions_matched + " / " + u.functions_total : "-"));
      tr.appendChild(cell("td", "n", u.functions_total ?
        u.functions_named + " / " + u.functions_total : "-"));
      tr.appendChild(cell("td", "n", txt(u.dp)));
      tr.appendChild(cell("td", "", u.complete ? "yes" : "no"));
      tbody.appendChild(tr);
      if (opened[u.i] && u.functions) tbody.appendChild(fnRows(u));
    });
    count.textContent = list.length + " of " + units.length + " units";
    drawMap();
  }

  // Treemap of units with code, squarified (Bruls et al.), largest first
  var cv = document.getElementById("mapcv");
  var tip = document.getElementById("tip");
  var legend = document.getElementById("legend");
  var STATES = [["done", "Fully matched"], ["part", "Partly matched"], ["none", "Not matched"]];
  var cells = [], laidOut = null;

  units.forEach(function (u) {
    u.state = u.code_total && u.code_matched >= u.code_total ? "done" :
              u.fuzzy > 0 ? "part" : "none";
  });

  function squarify(items, x, y, w, h) {
    var total = 0, out = [], i = 0;
    items.forEach(function (u) { total += u.code_total; });
    var scale = w * h / total;
    while (i < items.length) {
      var side = Math.min(w, h), sum = 0, best = Infinity, j = i;
      var first = items[i].code_total * scale;
      for (; j < items.length; j++) {
        var a = items[j].code_total * scale, s = sum + a;
        var worst = Math.max(side * side * first / (s * s), s * s / (side * side * a));
        if (worst > best) break;
        best = worst;
        sum = s;
      }
      var thick = sum / side, pos = 0;
      for (var k = i; k < j; k++) {
        var len = items[k].code_total * scale / thick;
        out.push(w >= h ? {u: items[k], x: x, y: y + pos, w: thick, h: len}
                        : {u: items[k], x: x + pos, y: y, w: len, h: thick});
        pos += len;
      }
      if (w >= h) { x += thick; w -= thick; } else { y += thick; h -= thick; }
      i = j;
    }
    return out;
  }

  function drawMap() {
    var c = cat.value, needle = q.value.trim().toLowerCase();
    var W = cv.clientWidth, H = cv.clientHeight, dpr = window.devicePixelRatio || 1;
    var key = c + "|" + W + "|" + H;
    var items = units.filter(function (u) {
      return u.code_total > 0 && (c === "all" || u.category === c);
    });
    if (laidOut !== key) {
      items.sort(function (a, b) { return b.code_total - a.code_total || a.i - b.i; });
      cells = items.length ? squarify(items, 0, 0, W, H) : [];
      laidOut = key;
    }
    cv.width = Math.round(W * dpr);
    cv.height = Math.round(H * dpr);
    var g = cv.getContext("2d"), css = getComputedStyle(document.documentElement);
    var color = {}, sums = {}, counts = {};
    STATES.forEach(function (s) {
      color[s[0]] = css.getPropertyValue("--map-" + s[0]).trim();
      sums[s[0]] = 0;
      counts[s[0]] = 0;
    });
    g.setTransform(dpr, 0, 0, dpr, 0, 0);
    g.fillStyle = css.getPropertyValue("--bg").trim();
    g.fillRect(0, 0, W, H);
    cells.forEach(function (r) {
      var gap = r.w > 3 && r.h > 3 ? 1 : 0;
      g.globalAlpha = !needle || r.u.name.toLowerCase().indexOf(needle) >= 0 ? 1 : 0.2;
      g.fillStyle = color[r.u.state];
      g.fillRect(r.x + gap / 2, r.y + gap / 2, r.w - gap, r.h - gap);
      sums[r.u.state] += r.u.code_total;
      counts[r.u.state]++;
    });
    g.globalAlpha = 1;
    var total = sums.done + sums.part + sums.none;
    legend.textContent = "";
    STATES.forEach(function (s) {
      var span = document.createElement("span");
      span.style.setProperty("--sw", "var(--map-" + s[0] + ")");
      span.textContent = s[1] + ": " + counts[s[0]] + " units, " +
        (total ? (100 * sums[s[0]] / total).toFixed(1) : "0.0") + "% of code";
      legend.appendChild(span);
    });
    cv.setAttribute("aria-label", "Map of " + cells.length + " units: " + legend.textContent);
  }

  function cellAt(ev) {
    var b = cv.getBoundingClientRect(), x = ev.clientX - b.left, y = ev.clientY - b.top;
    for (var i = 0; i < cells.length; i++) {
      var r = cells[i];
      if (x >= r.x && x < r.x + r.w && y >= r.y && y < r.y + r.h) return {r: r, x: x, y: y};
    }
    return null;
  }

  cv.addEventListener("mousemove", function (ev) {
    var hit = cellAt(ev);
    if (!hit) { tip.hidden = true; return; }
    var u = hit.r.u;
    tip.textContent = "";
    tip.appendChild(cell("b", "", u.name));
    tip.appendChild(document.createTextNode(
      num(u.code_total) + " bytes of code, " +
      (u.functions_total ? u.functions_matched + "/" + u.functions_total + " functions, " : "") +
      u.fuzzy.toFixed(2) + "% fuzzy"));
    tip.hidden = false;
    var mw = cv.clientWidth, tw = tip.offsetWidth, th = tip.offsetHeight;
    tip.style.left = Math.max(0, Math.min(hit.x + 12, mw - tw)) + "px";
    tip.style.top = (hit.y + 12 + th > cv.clientHeight ? hit.y - th - 8 : hit.y + 12) + "px";
  });
  cv.addEventListener("mouseleave", function () { tip.hidden = true; });
  cv.addEventListener("click", function (ev) {
    var hit = cellAt(ev);
    if (!hit) return;
    q.value = hit.r.u.name;
    render();
    document.getElementById("units").scrollIntoView({behavior: "smooth"});
  });
  window.addEventListener("resize", drawMap);
  if (window.matchMedia) {
    var mq = window.matchMedia("(prefers-color-scheme: dark)");
    if (mq.addEventListener) mq.addEventListener("change", drawMap);
  }

  document.querySelectorAll("#units thead th[data-k]").forEach(function (th) {
    th.addEventListener("click", function () {
      var k = th.getAttribute("data-k");
      if (key === k) { dir = -dir; } else { key = k; dir = 1; }
      render();
    });
  });
  q.addEventListener("input", render);
  cat.addEventListener("change", render);
  render();
})();
</script>
</body>
</html>
"""


def render_html(data) -> str:
    cards, bars = cards_and_bars(data)
    opts = "".join(f'<option value="{escape(c["id"])}">{escape(c["name"])}</option>'
                   for c in data["categories"])
    # '<' cannot occur in JSON syntax outside strings, so this stays valid
    blob = (json.dumps(data, sort_keys=True)
            .replace("<", "\\u003c").replace(">", "\\u003e").replace("&", "\\u0026"))
    return (PAGE.replace("@@GEN@@", escape(data["generated"]))
            .replace("@@CARDS@@", cards).replace("@@BARS@@", bars)
            .replace("@@CATOPTS@@", opts).replace("@@DATA@@", blob))


def dumps(obj) -> str:
    return json.dumps(obj, indent=1, sort_keys=True) + "\n"


def write_docs(data, report: Path):
    DOCS.mkdir(parents=True, exist_ok=True)
    (DOCS / "report.json").write_bytes(report.read_bytes())
    # data is loaded without functions here
    (DOCS / "summary.json").write_text(dumps(data))
    for name, b in badges(data).items():
        (DOCS / name).write_text(dumps(b))
    (DOCS / "index.html").write_text(render_html(data))
    print(f"wrote {DOCS.relative_to(ROOT)}/ (summary.json, badge-*.json, index.html, report.json)")


def update_readme(data):
    text = README.read_text()
    pat = re.compile(re.escape(START) + r".*?" + re.escape(END), re.S)
    if not pat.search(text):
        print("warning: README.md has no progress markers, table not updated",
              file=sys.stderr)
        return
    new = pat.sub(lambda _: START + "\n" + readme_table(data) + "\n" + END, text, count=1)
    README.write_text(new)
    print("updated the README progress table")


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("report", nargs="?", default=str(ROOT / "build" / "report.json"))
    ap.add_argument("--write", action="store_true",
                    help="refresh docs/progress/ and the README table")
    ap.add_argument("--html", metavar="PATH",
                    help="write a local page with each unit's functions")
    a = ap.parse_args()
    report = Path(a.report)
    if not report.exists():
        sys.exit(f"{report} not found: run `.venv/bin/ninja progress` first")

    data = load(report, detail=False)
    print(terminal(data))
    if a.write:
        write_docs(data, report)
        update_readme(data)
    if a.html:
        full = load(report, detail=True)
        out = Path(a.html)
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(render_html(full))
        print(f"wrote {a.html} (local, not for commit)")


if __name__ == "__main__":
    main()
