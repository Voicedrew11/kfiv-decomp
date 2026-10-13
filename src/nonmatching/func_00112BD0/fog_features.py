#!/usr/bin/env python3
"""Per-candidate measurements of the fog block, for decomp-permuter runs on src/nonmatching/func_00112BD0/fog_random.

As a hook (PERMUTER_FEATURE_HOOK=this file; run_fog.sh sets it) every scored candidate appends one line
to fog_random/features.csv:
  wrong      instructions of the 66-word fog block that differ from retail
  deps       dependencies (LOG_LINKS) of the four float loads in gcc's second scheduling pass; ours 0
  dep_kinds  the instruction patterns those loads depend on
  cvt_before cvt.s.w instructions ahead of each load (0x148/0x168/0x14C/0x16C); retail 8 each
  addiu_gap  instructions from `addiu $2,$9,0xB0` to the `sb $3,1($2)` that uses it; retail 2
  op_wrong   instructions whose opcode differs from retail (order only, registers ignored); base 23
  load_pos   1-based block positions of the four float loads (0x148/0x168/0x14C/0x16C); retail 40/43/45/47
  passes     the randomization passes applied to the candidate, in order
Rows go to features2.csv (features.csv holds runs from before op_wrong/load_pos). Candidates that
do better than the base on any measure are saved to fog_random/interesting/, one file
per behaviour (reasons, placement, dependency kinds), keeping the one with the fewest wrong.

    fog_features.py summary         tally features.csv by pass
    fog_features.py show [FILE...]  measures and source diff of saved candidates (default: all, best first)
    fog_features.py prune           reduce interesting/ to one file per behaviour
"""
import csv
import hashlib
import json
import os
import re
import struct
import sys
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
OUT_DIR = os.path.join(HERE, "fog_random")
CSV_PATH = os.path.join(OUT_DIR, "features2.csv")
BASELINE_PATH = os.path.join(OUT_DIR, "baseline2.json")
BLOCK_START, BLOCK_WORDS = 0x148, 66  # retail 0x1131DC-0x1132E0 in the cut-down object
LOAD_OFFSETS = (0x148, 0x168, 0x14C, 0x16C)  # retail issue order
ADDIU_B0 = 0x252200B0  # addiu $2, $9, 0xB0
SB_1_V0 = 0xA0430001  # sb $3, 0x1($2)
COLUMNS = ["score", "wrong", "op_wrong", "load_pos", "deps", "dep_kinds", "cvt_before", "addiu_gap", "passes", "hash"]

_target = None
_baseline = None


def block_words(obj):
    from elftools.elf.elffile import ELFFile

    with open(obj, "rb") as f:
        data = ELFFile(f).get_section_by_name(".text").data()
    end = min(len(data), BLOCK_START + BLOCK_WORDS * 4)
    return [struct.unpack("<I", data[k:k + 4])[0] for k in range(BLOCK_START, end, 4)]


def placement(words):
    """cvt.s.w count ahead of each float load, and the gap from the addiu to its first store."""
    cvt_seen, before = 0, {}
    addiu_idx, addiu_gap = -1, -1
    for idx, w in enumerate(words):
        if w >> 26 == 0x31 and (w >> 21) & 31 == 9 and (w & 0xFFFF) in LOAD_OFFSETS:
            before.setdefault(w & 0xFFFF, cvt_seen)
        if w >> 26 == 0x11 and (w >> 21) & 31 == 0x14 and w & 63 == 0x20:
            cvt_seen += 1
        if w == ADDIU_B0 and addiu_idx < 0:
            addiu_idx = idx
        if w == SB_1_V0 and addiu_idx >= 0 and addiu_gap < 0:
            addiu_gap = idx - addiu_idx
    return [before.get(off, -1) for off in LOAD_OFFSETS], addiu_gap


def opcode_order(words, target):
    """Instructions whose opcode differs from the target's, and the positions of the float loads."""
    from rabbitizer import Instruction

    op_wrong = sum(Instruction(a).getOpcodeName() != Instruction(b).getOpcodeName()
                   for a, b in zip(words, target)) + abs(len(target) - len(words))
    pos = {}
    for idx, w in enumerate(words):
        if w >> 26 == 0x31 and (w >> 21) & 31 == 9 and (w & 0xFFFF) in LOAD_OFFSETS:
            pos.setdefault(w & 0xFFFF, idx + 1)
    return op_wrong, [pos.get(off, -1) for off in LOAD_OFFSETS]


def load_distance(load_pos, retail_pos):
    """How far the loads sit from retail's positions (a missing load counts as 66)."""
    return sum(abs(p - r) if p > 0 else BLOCK_WORDS for p, r in zip(load_pos, retail_pos))


def behaviour_key(cvt_before, addiu_gap, dep_kinds, load_pos):
    return hashlib.sha1(f"{cvt_before} {addiu_gap} {dep_kinds} {load_pos}".encode()).hexdigest()[:12]


_INSN_HEAD = re.compile(r"^\((?:insn|jump_insn|call_insn)(?::\w+)? (\d+) ", re.M)
_LOAD = re.compile(r"\(set \(reg(?:/v)?:SF \d+ \$f\d+\)\s+\(mem/s:SF \(plus:SI \(reg:SI 9 t1\)\s+"
                   r"\(const_int (328|332|360|364)\)\)")
_CODE = re.compile(r"\) (-?\d+) \{([^}]*)\} ")


def load_dependencies(dump_path):
    """Dependencies of the last SF load from each fog offset, from a -dR dump."""
    try:
        text = open(dump_path).read()
    except OSError:
        return -1, ""
    heads = [(m.start(), int(m.group(1))) for m in _INSN_HEAD.finditer(text)]
    names, loads = {}, {}
    for i, (start, uid) in enumerate(heads):
        body = text[start:heads[i + 1][0] if i + 1 < len(heads) else len(text)]
        code = _CODE.search(body)
        names[uid] = code.group(2) if code else "?"
        load = _LOAD.search(body)
        if load and code:
            links = body[code.end():]
            # LOG_LINKS is the first s-expression after the insn code: (nil) or (insn_list ...)
            depth, end = 0, 0
            for j, ch in enumerate(links):
                depth += ch == "("
                depth -= ch == ")"
                if depth == 0:
                    end = j + 1
                    break
            loads[int(load.group(1))] = [int(u) for u in re.findall(r"insn_list (\d+)", links[:end])]
    if len(loads) < 4:
        return -1, ""
    deps = sorted({u for us in loads.values() for u in us})
    return len(deps), ";".join(sorted({names.get(u, "?") for u in deps}))


def _load_state():
    global _target, _baseline
    if _target is None:
        _target = block_words(os.path.join(HERE, "fog", "target.o"))
        os.makedirs(os.path.join(OUT_DIR, "interesting"), exist_ok=True)
    if _baseline is None and os.path.exists(BASELINE_PATH):
        _baseline = json.load(open(BASELINE_PATH))


def _append(row):
    line = ",".join(str(row[c]) for c in COLUMNS) + "\n"
    new = not os.path.exists(CSV_PATH)
    fd = os.open(CSV_PATH, os.O_WRONLY | os.O_APPEND | os.O_CREAT, 0o644)
    try:
        os.write(fd, ((",".join(COLUMNS) + "\n") if new else "").encode() + line.encode())
    finally:
        os.close(fd)


def record(o_file, source, applied, score):
    _load_state()
    words = block_words(o_file)
    wrong = sum(a != b for a, b in zip(words, _target)) + abs(len(_target) - len(words))
    deps, dep_kinds = load_dependencies(o_file[: -len(".o")] + ".sched2")
    cvt_before, addiu_gap = placement(words)
    op_wrong, load_pos = opcode_order(words, _target)
    digest = hashlib.sha1(struct.pack(f"<{len(words)}I", *words) + dep_kinds.encode()).hexdigest()[:12]
    row = dict(score=score, wrong=wrong, op_wrong=op_wrong, load_pos="/".join(map(str, load_pos)), deps=deps, dep_kinds=dep_kinds or "-",
               cvt_before="/".join(map(str, cvt_before)), addiu_gap=addiu_gap,
               passes="+".join(applied) or "base", hash=digest)
    global _baseline
    if not applied and _baseline is None:
        retail_cvt, retail_gap = placement(_target)
        retail_pos = opcode_order(_target, _target)[1]
        _baseline = dict(wrong=wrong, op_wrong=op_wrong, load_dist=load_distance(load_pos, retail_pos), deps=deps,
                         cvt_sum=sum(cvt_before), addiu_gap=addiu_gap,
                         retail=dict(cvt_before=retail_cvt, addiu_gap=retail_gap, load_pos=retail_pos))
        with open(BASELINE_PATH, "w") as f:
            json.dump(_baseline, f, indent=1)
    _append(row)
    if not applied or _baseline is None:
        return
    reasons = [name for name, better in (
        ("deps", deps > _baseline["deps"]),
        ("loads-later", sum(cvt_before) > _baseline["cvt_sum"] and min(cvt_before) >= 0),
        ("addiu-closer", 0 < addiu_gap < _baseline["addiu_gap"]),
        ("ops-closer", op_wrong < _baseline["op_wrong"]),
        ("loads-closer", load_distance(load_pos, _baseline["retail"]["load_pos"]) < _baseline["load_dist"]),
        ("fewer-wrong", wrong < _baseline["wrong"]),
    ) if better]
    if reasons:
        # one file per behaviour (reasons, placement, dependency kinds), keeping the fewest wrong
        prefix = "+".join(reasons)
        behaviour = behaviour_key(row["cvt_before"], addiu_gap, dep_kinds, row["load_pos"])
        folder = os.path.join(OUT_DIR, "interesting")
        same = [n for n in os.listdir(folder) if n.startswith(prefix + "-w") and n.endswith(f"-{behaviour}.c")]
        if any(int(re.search(r"-w(\d+)-", n).group(1)) <= wrong for n in same):
            return
        header = "".join(f"// {c}: {row[c]}\n" for c in COLUMNS)
        name = f"{prefix}-w{wrong}-d{deps}-{behaviour}.c"
        with open(os.path.join(folder, name), "w") as f:
            f.write(header + source)
        if "fewer-wrong" in reasons:
            _announce_best(wrong, name)
        for n in same:
            try:
                os.remove(os.path.join(folder, n))
            except OSError:
                pass


def summary():
    rows = list(csv.DictReader(open(CSV_PATH)))
    base = json.load(open(BASELINE_PATH))
    print(f"{len(rows)} candidates; base: wrong {base['wrong']}, op_wrong {base['op_wrong']}, "
          f"load distance {base['load_dist']}, deps {base['deps']}, "
          f"cvt_before sum {base['cvt_sum']}, addiu gap {base['addiu_gap']}; retail: {base['retail']}")
    best = min(rows, key=lambda r: int(r["wrong"]))
    print(f"fewest wrong: {best['wrong']} (score {best['score']}, hash {best['hash']})")
    best = min(rows, key=lambda r: (int(r["op_wrong"]), int(r["wrong"])))
    print(f"fewest op_wrong: {best['op_wrong']} (wrong {best['wrong']}, loads {best['load_pos']}, hash {best['hash']})")
    counts = defaultdict(lambda: [0, 0, 0, 0, 0, 0, 0])
    for r in rows:
        cvt = [int(x) for x in r["cvt_before"].split("/")]
        flags = (int(r["deps"]) > base["deps"], sum(cvt) > base["cvt_sum"] and min(cvt) >= 0,
                 0 < int(r["addiu_gap"]) < base["addiu_gap"], int(r["op_wrong"]) < base["op_wrong"],
                 load_distance([int(x) for x in r["load_pos"].split("/")], base["retail"]["load_pos"]) < base["load_dist"],
                 int(r["wrong"]) < base["wrong"])
        for p in set(r["passes"].split("+")):
            c = counts[p]
            c[0] += 1
            for k, f in enumerate(flags):
                c[k + 1] += f
    print(f"\n{'pass':34} {'tried':>8} {'deps+':>7} {'loads>':>7} {'addiu<':>7} {'ops<':>7} {'ldist<':>7} {'wrong<':>7}")
    for p, c in sorted(counts.items(), key=lambda kv: -sum(kv[1][1:])):
        print(f"{p:34} {c[0]:8} " + " ".join(f"{n:7}" for n in c[1:]))
    kinds = defaultdict(int)
    for r in rows:
        if r["dep_kinds"] not in ("-", ""):
            kinds[r["dep_kinds"]] += 1
    if kinds:
        print("\nload dependencies seen:")
        for k, n in sorted(kinds.items(), key=lambda kv: -kv[1])[:15]:
            print(f"{n:8}  {k}")


def _announce_best(wrong, name):
    """Banner in the run's terminal, a line in fog_random/NEW_BEST.txt and a desktop notification."""
    import shutil
    import subprocess
    import time

    message = f"NEW BEST: {wrong} wrong (base {_baseline['wrong']}): interesting/{name}"
    print("\n" + "*" * len(message) + "\n" + message + "\n" + "*" * len(message), file=sys.stderr, flush=True)
    with open(os.path.join(OUT_DIR, "NEW_BEST.txt"), "a") as f:
        f.write(f"{time.strftime('%Y-%m-%d %H:%M:%S')}  {message}\n")
    if shutil.which("notify-send"):
        subprocess.Popen(["notify-send", "-u", "critical", "Fog permuter: new best", message],
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)


def prune(folder=os.path.join(OUT_DIR, "interesting")):
    """Reduce saved candidates to one per behaviour, renamed the way record() names them."""
    best = {}
    names = [n for n in os.listdir(folder) if n.endswith(".c")]
    for n in names:
        head = dict(l[3:].split(": ", 1) for l in open(os.path.join(folder, n)).read().splitlines()[:len(COLUMNS)]
                    if l.startswith("// ") and ": " in l)
        dep_kinds = "" if head["dep_kinds"] == "-" else head["dep_kinds"]
        behaviour = behaviour_key(head["cvt_before"], head["addiu_gap"], dep_kinds, head.get("load_pos", ""))
        key = (re.match(r"(.*)-w\d+-", n).group(1), behaviour)
        entry = (int(head["wrong"]), n, f"{key[0]}-w{head['wrong']}-d{head['deps']}-{behaviour}.c")
        if key not in best or entry < best[key]:
            best[key] = entry
    keep = {old: new for _, old, new in best.values()}
    for n in names:
        if n not in keep:
            os.remove(os.path.join(folder, n))
        elif keep[n] != n:
            os.rename(os.path.join(folder, n), os.path.join(folder, keep[n]))
    print(f"{len(names)} files -> {len(keep)}")


def _printed_base():
    """base.c printed the way the permuter prints candidates, so diffs show only real changes."""
    permuter_dir = os.path.join(HERE, "..", "..", "tools", "cc", "decomp-permuter")
    sys.path.insert(0, permuter_dir)
    from src import ast_util

    source = open(os.path.join(OUT_DIR, "base.c")).read()
    source = source.replace("PERM_RANDOMIZE(\n", "").replace("                )\n            }", "            }")
    return ast_util.to_c(ast_util.parse_c(source))


def show(paths):
    import difflib

    if not paths:
        def rank(path):
            name = os.path.basename(path)
            wrong = int(re.search(r"-w(\d+)-", name).group(1))
            return (-name.count("+") - ("deps" in name), wrong)
        paths = sorted((os.path.join(OUT_DIR, "interesting", n) for n in os.listdir(os.path.join(OUT_DIR, "interesting"))),
                       key=rank)
    base = _printed_base().splitlines()
    for path in paths:
        lines = open(path).read().splitlines()
        header = [l for l in lines[:len(COLUMNS)] if l.startswith("// ")]
        body = lines[len(header):]
        print("=" * 100)
        print(os.path.basename(path))
        for l in header:
            if not l.startswith("// hash"):
                print("  " + l[3:])
        for l in difflib.unified_diff(base, body, lineterm="", n=1):
            if not l.startswith(("---", "+++")):
                print("  " + l)


if __name__ == "__main__":
    if sys.argv[1:] == ["summary"]:
        summary()
    elif sys.argv[1:] == ["prune"]:
        prune()
    elif sys.argv[1:2] == ["show"]:
        show(sys.argv[2:])
    else:
        print(__doc__)
