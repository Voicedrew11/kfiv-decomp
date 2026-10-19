"""Apply named source edits to src/nonmatching/Map_UpdateDrawState.c and score each via a temp copy in src/game."""
import subprocess, sys
BASE = open("src/nonmatching/Map_UpdateDrawState.c").read()
RED = "blend_sum = FOG_COLOR(&D_0038A1B0.unk140)->red * fade_out + FOG_COLOR(&D_0038A1B0.unk160)->red * fade_in;"
FAR = "far_to = D_0038A1B0.unk160.unk0C;"
NEAR = "D_0038A1B0.unkA0.unk0C = D_0038A1B0.unk140.unk0C * fade_out + far_to;"
LAST = "blend_sum = (blue_to = D_0038A1B0.unk160.unk08 * fade_in + D_0038A1B0.unk140.unk08 * fade_out);"
EDITS = {
    "red_swap": [(RED, "blend_sum = fade_out * FOG_COLOR(&D_0038A1B0.unk140)->red + FOG_COLOR(&D_0038A1B0.unk160)->red * fade_in;")],
    "near_split": [(NEAR, "D_0038A1B0.unkA0.unk0C = D_0038A1B0.unk140.unk0C;\n D_0038A1B0.unkA0.unk0C = D_0038A1B0.unkA0.unk0C * fade_out + far_to;")],
    "far_chain": [(FAR, "far_to = (blue_from = D_0038A1B0.unk160.unk0C);")],
    "near_temp": [(FAR, FAR + "\n blue_from = D_0038A1B0.unk140.unk0C;"), (NEAR, "D_0038A1B0.unkA0.unk0C = blue_from * fade_out + far_to;")],
    "unk08_temp_green": [(LAST, "green_to = D_0038A1B0.unk140.unk08;\n blend_sum = (blue_to = D_0038A1B0.unk160.unk08 * fade_in + green_to * fade_out);")],
    "unk08_temp_blue": [(LAST, "blue_from = D_0038A1B0.unk160.unk08;\n blend_sum = (blue_to = blue_from * fade_in + D_0038A1B0.unk140.unk08 * fade_out);")],
    "store_chain": [(LAST + "\n                D_0038A1B0.unkA0.unk08 = blend_sum;", "blend_sum = (blue_to = (D_0038A1B0.unkA0.unk08 = D_0038A1B0.unk160.unk08 * fade_in + D_0038A1B0.unk140.unk08 * fade_out));")],
}
def score(src):
    open("src/game/Map_UpdateDrawState.c", "w").write(src)
    try:
        out = subprocess.run([".venv/bin/python", "tools/match.py", "diff", "src/game/Map_UpdateDrawState.c"], capture_output=True, text=True).stdout
    finally:
        subprocess.run(["rm", "-f", "src/game/Map_UpdateDrawState.c"])
    return out.splitlines()[0] if out else "compile failed"
names = sys.argv[1:] or ["base"] + list(EDITS)
for spec in names:
    src = BASE
    for name in spec.split("+"):
        for old, new in EDITS.get(name, []):
            assert src.count(old) == 1, (name, old)
            src = src.replace(old, new)
    print(f"{spec:40} {score(src)}")
