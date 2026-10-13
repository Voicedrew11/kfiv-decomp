"""Fog block rewritten in retail's split order (conversions, red sum and green multiplies first), with barriers."""
import importlib.util, itertools, sys
sys.argv = ["x", "show", "0", "0"]
spec = importlib.util.spec_from_file_location("tb", "src/nonmatching/func_00112BD0/try_barrier.py")
tb = importlib.util.module_from_spec(spec)
src = open("src/nonmatching/func_00112BD0.c").read()
I = "                "
a = src.index(I + "blend_sum = FOG_COLOR(&D_0038A1B0.unk140)->red")
b = src.index(I + "far_to = D_0038A1B0.unk160.unk0C;")
BEFORE = [
    "blend_sum = FOG_COLOR(&D_0038A1B0.unk140)->red * fade_out + FOG_COLOR(&D_0038A1B0.unk160)->red * fade_in;",
    "green_from = FOG_COLOR(&D_0038A1B0.unk140)->green;",
    "green_to = FOG_COLOR(&D_0038A1B0.unk160)->green;",
    "green_from *= fade_out;",
    "green_to *= fade_in;",
    "blue_from = FOG_COLOR(&D_0038A1B0.unk140)->blue;",
    "blue_to = FOG_COLOR(&D_0038A1B0.unk160)->blue;",
]
AFTER = [
    "blended = (int)(blend_sum);",
    "FOG_COLOR(&D_0038A1B0.unkA0)->red = blended;",
    "green_from += green_to;",
    "blended = (int)(green_from);",
    "FOG_COLOR(&D_0038A1B0.unkA0)->green = blended;",
    "blue_from *= fade_out;",
    "blue_to *= fade_in;",
    "blue_from += blue_to;",
    "blended = (int)(blue_from);",
    "FOG_COLOR(&D_0038A1B0.unkA0)->blue = blended;",
]
exec(open("src/nonmatching/func_00112BD0/try_barrier.py").read().split("def label")[0].split("lines = open")[0])
exec("def score(src):" + open("src/nonmatching/func_00112BD0/try_barrier.py").read().split("def score(src):")[1].split("def label")[0])
stmts = BEFORE + AFTER
best = []
for kind in (0, 1):
    for r in (0, 1, 2):
        for combo in itertools.combinations(range(len(stmts) + 1), r):
            body = ""
            for i, s in enumerate(stmts + [None]):
                if i in combo:
                    body += I + BARRIERS[kind] + "\n"
                if s:
                    body += I + s + "\n"
            sc = score(src[:a] + body + src[b:])
            best.append((sc, kind, combo))
            if r == 0 or (len(best) and sc <= 31):
                print(sc, kind, combo, flush=True)
best.sort()
print("best", best[:8])
