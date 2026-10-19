"""Insert unused user labels at statement boundaries of the fog block and score each placement."""
import itertools, subprocess, sys

SRC = sys.argv[1] if len(sys.argv) > 1 else "src/nonmatching/Map_UpdateDrawState.c"
TMP = "src/game/Map_UpdateDrawState.c"
FIRST, LAST = "fade_out = D_0038A1B0.unk9A;", "D_0038A1B0.unkA0.unk08 = blend_sum;"

lines = open(SRC).read().split("\n")
start = next(i for i, l in enumerate(lines) if l.strip() == FIRST)
end = next(i for i, l in enumerate(lines) if l.strip() == LAST)
positions = [i for i in range(start, end + 2) if i > end or not lines[i].strip().startswith("/*")]


def build(pos_list):
    out = []
    for i, l in enumerate(lines):
        if i in pos_list:
            out.append(f"            split{pos_list.index(i)}:; {{ static void *anchor{pos_list.index(i)} = &&split{pos_list.index(i)}; }}")
        out.append(l)
    return "\n".join(out)


def score(src):
    open(TMP, "w").write(src)
    try:
        r = subprocess.run([".venv/bin/python", "tools/match.py", "diff", TMP], capture_output=True, text=True)
    finally:
        subprocess.run(["rm", "-f", TMP])
    first = r.stdout.splitlines()[:1]
    if first and "differ" in first[0]:
        return int(first[0].split(":")[1].split()[0])
    if first and ": OK (" in first[0]:
        return 0
    return 9999


RS = [int(x) for x in (sys.argv[2] if len(sys.argv) > 2 and sys.argv[2] != "show" else "1").split(",")]
if sys.argv[2:3] == ["show"]:
    print(build([int(p) for p in sys.argv[3].split(",")]))
    sys.exit()

print("base", score(build([])), flush=True)
results = []
for r in RS:
    for combo in itertools.combinations(positions, r):
        s = score(build(list(combo)))
        results.append((s, combo))
        if r == 1 or s < 31:
            print(f"{s:4} {','.join(f'{p}:{lines[p].strip()[:40] if p <= end else chr(60)+chr(101)+chr(110)+chr(100)+chr(62)}' for p in combo)}", flush=True)
results.sort()
print("best", results[:8])
