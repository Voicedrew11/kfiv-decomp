"""Insert empty asm volatile barriers at statement boundaries of the fog block and score each placement.

    try_barrier.py            singles and pairs, both barrier kinds -> build/permuter/barrier_results.txt
    try_barrier.py show N,M K print the source with barriers at positions N,M of kind K (0 plain, 1 memory)
"""
import itertools, os, subprocess, sys

SRC = "src/nonmatching/func_00112BD0.c"
TMP = "src/game/func_00112BD0.c"
BARRIERS = ['__asm__ volatile("");', '__asm__ volatile("" : : : "memory");']
FIRST, LAST = "fade_out = D_0038A1B0.unk9A;", "D_0038A1B0.unkA0.unk08 = blend_sum;"

lines = open(SRC).read().split("\n")
start = next(i for i, l in enumerate(lines) if l.strip() == FIRST)
end = next(i for i, l in enumerate(lines) if l.strip() == LAST)
# Position p means "before line p"; end + 1 is after the last statement.
positions = [i for i in range(start, end + 2) if i > end or not lines[i].strip().startswith("/*")]


def build(pos_list, kind):
    out = []
    for i, l in enumerate(lines):
        if i in pos_list:
            out.append("                " + BARRIERS[kind])
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


def label(pos_list):
    return ",".join(f"{p}:{lines[p].strip()[:30] if p <= end else '<end>'}" for p in pos_list)


if sys.argv[1:2] == ["show"]:
    print(build([int(p) for p in sys.argv[2].split(",")], int(sys.argv[3])))
    sys.exit()

print("base", score(build([], 0)), flush=True)
results = []
specs = [(c, k) for k in (0, 1) for r in (1, 2) for c in itertools.combinations(positions, r)]
for n, (combo, kind) in enumerate(specs):
    s = score(build(list(combo), kind))
    results.append((s, kind, combo))
    if s < 31:
        print(f"{s:4} kind={kind} {label(combo)}", flush=True)
results.sort()
os.makedirs("build/permuter", exist_ok=True)
with open("build/permuter/barrier_results.txt", "w") as f:
    for s, kind, combo in results:
        f.write(f"{s:4} kind={kind} {','.join(map(str, combo))}  {label(combo)}\n")
print("done", len(results), "best", results[:5])
