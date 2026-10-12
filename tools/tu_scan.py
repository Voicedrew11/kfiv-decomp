#!/usr/bin/env python3
"""Find which game functions must share a translation unit.

    tools/tu_scan.py                      summary; writes build/tu_groups.txt
    tools/tu_scan.py --check START END    may [START, END) be one unit?

KFIV's game .text keeps no trace of where one object file ends: ee-as pads
every function to 8 bytes, so there is no padding at object ends, and there
is no link map or symbol table. What the binary does fix are pairs of
functions that must share an object, from the data each object keeps to
itself:

  * string literals and jump tables (.rodata, and strings of 8 bytes or less
    in .sdata; tables of game code addresses count as jump tables). gcc emits
    each distinct string once per object.
  * .bss and .sbss. The game has no COMMON block and its .bss precedes the
    library's, so these hold only file-static variables. (Its uninitialized
    globals are in .data/.sdata, as with -fno-common.) An .sbss item is its
    exact address: splat writes neighbouring small statics as "D_X + 0xC",
    which in .bss is more often a field of one object.

Every function that uses one such item is in the object that holds it, and
since objects keep the link order in every section, an item placed after
another but first used before the other's last use ties every function in
between to one object. Each of these is a "group": a run of functions no
unit boundary may split. Two copies of one string are in two objects, so a
boundary lies between their users ("must cut").

Any partition into units that keeps every group whole and makes every must
cut builds the same bytes. The true object files are some coarsening of the
groups; statics are emitted in declaration order, which follows use order, so
the address order of the data cannot place the boundaries themselves.

Needs asm/ (run ./build.sh first).
"""
import argparse
import collections
import glob
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lib_layout import ROOT, load_retail, load_symbols  # noqa: E402

ROM_BASE = 0x00100000
YAML = os.path.join(ROOT, "build/kfiv.us.yaml")
OUT = os.path.join(ROOT, "build/tu_groups.txt")
GP = 0x0044CB70
INSN = re.compile(r"/\* [0-9A-F]+ ([0-9A-F]{8}) [0-9A-F]{8} \*/\s+(\S+)\s*(.*)")
REF = re.compile(r"\b((?:D|jtbl)_([0-9A-F]{8}))\b(?:\s*\+\s*(0x[0-9A-Fa-f]+))?")


def game_range():
    """From the first .text subsegment after crt0 to the first library unit."""
    lo = hi = None
    for line in open(YAML):
        m = re.match(r"\s+- \[0x([0-9A-F]+), (\w+), ([\w/]+)\]", line)
        if not m:
            continue
        addr, name = int(m[1], 16) + ROM_BASE, m[3]
        if name == "sce/crt0":
            continue
        if lo is None:
            lo = addr
        elif name.startswith("sce/"):
            hi = addr
            break
    return lo, hi


def load_functions(lo, hi):
    """[(name, start, end, [(addr, op, args)])] from splat's asm, game range only."""
    files = sorted(glob.glob(os.path.join(ROOT, "asm/text*.s")))
    files += sorted(glob.glob(os.path.join(ROOT, "asm/matchings/game/**/*.s"), recursive=True))
    if not files:
        sys.exit("tu_scan: no asm/ (run ./build.sh)")
    funcs = {}
    for fn in files:
        cur = None
        for line in open(fn):
            if line.startswith("glabel "):
                cur = line.split()[1]
            elif line.startswith("endlabel "):
                cur = None
            elif cur:
                m = INSN.search(line)
                if m and lo <= int(m[1], 16) < hi:
                    funcs.setdefault(cur, []).append((int(m[1], 16), m[2], m[3]))
    out = [(name, ins[0][0], ins[-1][0] + 4, ins) for name, ins in funcs.items()]
    return sorted(out, key=lambda f: f[1])


def read(retail, addr, n):
    for base, data in retail.values():
        if base <= addr < base + len(data):
            return data[addr - base:addr - base + n]
    return b""


def cstring(retail, addr):
    """The string gcc would emit at `addr` (8-aligned; ASCII and EUC-JP), or None."""
    if addr % 8:
        return None
    b = read(retail, addr, 512)
    end = b.find(b"\0")
    # the next item is 8-aligned: a string's NUL is followed by zeros up to it
    if end <= 0 or b[end:(end + 8) & ~7].strip(b"\0"):
        return None
    b, i = b[:end], 0
    while i < len(b):
        c = b[i]
        if 32 <= c < 127 or c in (9, 10, 13):
            i += 1
        elif (0xA1 <= c <= 0xFE or c == 0x8E) and i + 1 < len(b) and 0xA1 <= b[i + 1] <= 0xFE:
            i += 2
        else:
            return None
    return b


def code_table(retail, addr, lo, hi):
    """True if `addr` starts at least two words that all point into the game's .text."""
    b = read(retail, addr, 8)
    words = [int.from_bytes(b[k:k + 4], "little") for k in range(0, len(b) - 3, 4)]
    return len(words) == 2 and all(lo <= w < hi for w in words)


def section_of(retail, addr):
    for name, (base, data) in retail.items():
        if base <= addr < base + len(data):
            return name
    return None


def scan():
    lo, hi = game_range()
    funcs = load_functions(lo, hi)
    retail = load_retail(os.path.join(ROOT, "baserom/us/SLUS_203.18"))
    # private items per sequence: (sequence) -> addr -> {function index}
    seqs = collections.defaultdict(lambda: collections.defaultdict(set))
    kind = {}
    for i, (_, _, _, ins) in enumerate(funcs):
        for _, op, args in ins:
            if op in ("jal", "j") or op.startswith("b"):
                continue
            for m in REF.finditer(args):
                t = int(m[2], 16)
                # .sbss objects are at most 8 bytes (-G 8), so splat's "D_X + 0xC" there is its own variable
                if m[3] and section_of(retail, t + int(m[3], 16)) == ".sbss":
                    t += int(m[3], 16)
                if t == GP:
                    continue
                sec = section_of(retail, t)
                if m[1].startswith("jtbl") or (sec == ".rodata" and code_table(retail, t, lo, hi)):
                    seqs[".rodata"][t].add(i); kind[t] = "jump table"
                elif sec in (".bss", ".sbss"):
                    seqs[sec][t].add(i); kind[t] = "static"
                elif sec in (".rodata", ".sdata") and cstring(retail, t) is not None:
                    seqs[sec][t].add(i); kind[t] = "string"
    n = len(funcs)
    tie = [None] * n          # tie[c]: why functions c and c+1 must share a unit
    def join(a, b, why):
        for c in range(a, b):
            if tie[c] is None:
                tie[c] = why
    for sec, items in seqs.items():
        run, run_item = -1, None
        for t, users in sorted(items.items()):
            first, last = min(users), max(users)
            join(first, last, f"{kind[t]} {t:#010x} used by {funcs[first][0]} .. {funcs[last][0]}")
            if first < run:
                join(first, run, f"{sec} order: {t:#010x} first used before {run_item:#010x} is last used")
            if last > run:
                run, run_item = last, t
    must = []
    for sec in (".rodata", ".sdata"):
        copies = collections.defaultdict(list)
        for t, users in sorted(seqs[sec].items()):
            if kind[t] == "string":
                copies[cstring(retail, t)].append(users)
        for s, uses in copies.items():
            for a, b in zip(uses, uses[1:]):
                must.append((max(a), min(b), s.decode("latin-1")))
    return funcs, tie, must


def groups_of(funcs, tie):
    out, start = [], 0
    for c in range(len(funcs)):
        if c == len(funcs) - 1 or tie[c] is None:
            out.append((start, c))
            start = c + 1
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--check", nargs=2, metavar=("START", "END"),
                    help="report whether [START, END) keeps every group whole")
    args = ap.parse_args()
    funcs, tie, must = scan()
    groups = groups_of(funcs, tie)
    for lo, hi, s in must:
        if all(tie[c] for c in range(lo, hi)):
            print(f"tu_scan: contradiction: two copies of {s!r} but no allowed cut "
                  f"between {funcs[lo][0]} and {funcs[hi][0]}", file=sys.stderr)
    if args.check:
        a, b = (int(x, 0) for x in args.check)
        idx = {f[1]: i for i, f in enumerate(funcs)}
        bad = 0
        for edge, side in ((a, "start"), (b, "end")):
            if edge not in idx:
                if side == "end" and edge == funcs[-1][2]:
                    continue
                print(f"{side} {edge:#010x} is not a game function start"); bad += 1
            elif idx[edge] > 0 and tie[idx[edge] - 1]:
                print(f"{side} {edge:#010x} splits a group: {tie[idx[edge] - 1]}"); bad += 1
        if a in idx and b in idx:
            for lo, hi, s in must:
                if idx[a] <= lo and hi < idx[b]:
                    print(f"two copies of {s!r} ({funcs[lo][0]}, {funcs[hi][0]}) need a cut inside"); bad += 1
        print("ok" if not bad else f"{bad} problem(s)")
        sys.exit(1 if bad else 0)
    sizes = collections.Counter(hi - lo + 1 for lo, hi in groups)
    multi = [g for g in groups if g[1] > g[0]]
    print(f"{len(funcs)} game functions, {len(groups)} groups "
          f"({len(multi)} of two or more, covering {sum(h - l + 1 for l, h in multi)} functions); "
          f"largest {max(sizes)}; {len(must)} must-cut ranges")
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w") as f:
        f.write("# tools/tu_scan.py: runs of game functions that must share a unit\n")
        f.write("# start end functions first-function last-function / why the run is tied\n")
        for lo, hi in groups:
            f.write(f"{funcs[lo][1]:#010x} {funcs[hi][2]:#010x} {hi - lo + 1:4d} "
                    f"{funcs[lo][0]} {funcs[hi][0]}\n")
            for c in range(lo, hi):
                if c == lo or tie[c] != tie[c - 1]:
                    f.write(f"    {tie[c]}\n")
        f.write("\n# must cut: two copies of a string, so a unit boundary lies in (after, before]\n")
        for lo, hi, s in must:
            f.write(f"after {funcs[lo][0]} before {funcs[hi][0]}: {s!r}\n")
    print(f"wrote {os.path.relpath(OUT, ROOT)}")


if __name__ == "__main__":
    main()
