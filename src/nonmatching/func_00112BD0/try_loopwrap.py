"""Wrap each contiguous range of fog-block statements in do { } while (0) and score (stray loop alignment removed).

try_loopwrap.py [SRC]            all ranges
try_loopwrap.py SRC show A B     print the source with lines [A, B) wrapped
"""
import os, re, subprocess, sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../.."))
os.chdir(ROOT)
sys.path.insert(0, "tools")
import match

SRC = sys.argv[1] if len(sys.argv) > 1 else "src/nonmatching/Map_UpdateDrawState.c"
TMP = "src/game/Map_UpdateDrawState.c"
OUT = "build/match/src/game/Map_UpdateDrawState.c.o"
FIRST = "fade_out = D_0038A1B0.unk9A;"
lines = open(SRC).read().split("\n")
start = next(i for i, l in enumerate(lines) if l.strip() == FIRST)
end = next(i for i, l in enumerate(lines) if i > start and l.strip() == "}")  # closes the fog block
stmts = [i for i in range(start, end) if not lines[i].strip().startswith("/*")]


def build(a, b):
    out = []
    for i, l in enumerate(lines):
        if i == a:
            out.append("                do {")
        if i == b:
            out.append("                } while (0);")
        out.append(l)
    return "\n".join(out)


def unalign(s):
    # the stray .p2align 3 lands on the first label after the fog block (the else branch's copy)
    return re.sub(r"\t\.p2align 3\n(\$L\d+:\n\tld\t\$3,352\(\$9\))", r"\t.p2align 2\n\1", s)


def score(src):
    open(TMP, "w").write(src)
    try:
        if subprocess.run(["tools/compile_c.sh", TMP, OUT], capture_output=True).returncode:
            return 9999
        s_path = OUT[:-2] + ".s"
        text = unalign(open(s_path).read())
        open(s_path, "w").write(text)
        subprocess.run(["tools/period_env.sh", "build/ee-as-2.9-fill", "-EL", "-mcpu=5900", "-mabi=eabi", "-G", "8",
                        "-o", OUT, s_path], check=True)
        r = subprocess.run([".venv/bin/python", "-c",
                            "import sys,subprocess; sys.path.insert(0,'tools'); import match;"
                            "subprocess.run=lambda *a,**k: subprocess.CompletedProcess(a,0); match.diff(sys.argv[1])", TMP],
                           capture_output=True, text=True)
    finally:
        os.remove(TMP)
    first = r.stdout.splitlines()[:1]
    if first and "differ" in first[0]:
        return int(first[0].split(":")[1].split()[0])
    if first and ": OK (" in first[0]:
        return 0
    if os.environ.get("DEBUG"):
        print(r.stdout[-500:], r.stderr[-1500:])
    return 9999


if sys.argv[2:3] == ["show"]:
    print(build(int(sys.argv[3]), int(sys.argv[4])))
    sys.exit()
print("base", score(build(-1, -1)), flush=True)
if os.environ.get("DEBUG"):
    sys.exit()
res = []
ends = stmts[1:] + [end]
for ai, a in enumerate(stmts):
    for b in ends[ai:]:
        s = score(build(a, b))
        res.append((s, a, b))
        print(f"{s:5} [{a},{b})  {lines[a].strip()[:35]}  ..  {lines[b].strip()[:35]}", flush=True)
res.sort()
print("best", res[:10])
