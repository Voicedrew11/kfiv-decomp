"""Fog block in retail's split order, with a loop wrapper (loop notes = haifa-sched barrier) around one half."""
import subprocess, sys

SRC = "src/nonmatching/Map_UpdateDrawState.c"
TMP = "src/game/Map_UpdateDrawState.c"
text = open(SRC).read()
start = text.index("            { /* fog blend")
end = text.index("        } else {\n            D_0038A1B0.unkA0 = D_0038A1B0.unk160;")

DECL = """                f32 red_from, red_to, green_from, green_to, blue_from, blue_to;
                fade_out = D_0038A1B0.unk9A;
                fade_in = 30 - D_0038A1B0.unk9A;
                fade_out /= 30.0f;
                fade_in /= 30.0f;
"""
CONV = """red_from = FOG_COLOR(&D_0038A1B0.unk140)->red;
red_to = FOG_COLOR(&D_0038A1B0.unk160)->red;
green_from = FOG_COLOR(&D_0038A1B0.unk140)->green;
green_to = FOG_COLOR(&D_0038A1B0.unk160)->green;
blue_to = FOG_COLOR(&D_0038A1B0.unk160)->blue;
blue_from = FOG_COLOR(&D_0038A1B0.unk140)->blue;
red_from = red_from * fade_out + red_to * fade_in;
green_from *= fade_out;
green_to *= fade_in;
"""
REST = """FOG_COLOR(&D_0038A1B0.unkA0)->red = (int)red_from;
green_from += green_to;
FOG_COLOR(&D_0038A1B0.unkA0)->green = (int)green_from;
blue_from *= fade_out;
blue_to *= fade_in;
blue_from += blue_to;
FOG_COLOR(&D_0038A1B0.unkA0)->blue = (int)blue_from;
D_0038A1B0.unkA0.unk0C = D_0038A1B0.unk140.unk0C * fade_out + D_0038A1B0.unk160.unk0C * fade_in;
D_0038A1B0.unkA0.unk08 = D_0038A1B0.unk140.unk08 * fade_out + D_0038A1B0.unk160.unk08 * fade_in;
"""


def ind(s):
    return "".join("                " + l + "\n" for l in s.strip().split("\n"))


WRAPS = {
    "none": lambda s: ind(s),
    "do-while0": lambda s: "                do {\n" + ind(s) + "                } while (0);\n",
    "for-break": lambda s: "                for (;;) {\n" + ind(s) + "                    break;\n                }\n",
    "while1-break": lambda s: "                while (1) {\n" + ind(s) + "                    break;\n                }\n",
}


def variant(wrap_first, wrap_rest):
    body = DECL + WRAPS[wrap_first](CONV) + WRAPS[wrap_rest](REST)
    return text[:start] + "            {\n" + body + "            }\n" + text[end:]


def score(src, dump=False):
    open(TMP, "w").write(src)
    try:
        r = subprocess.run([".venv/bin/python", "tools/match.py", "diff", TMP], capture_output=True, text=True)
        notes = ""
        if dump:
            subprocess.run(["tools/compile_c.sh", TMP, "build/match/_lp.o"], env={**__import__("os").environ, "GAME_CFLAGS": "-G 8 -dR"}, capture_output=True)
            d = open("src/game/Map_UpdateDrawState.c.sched2").read()
            notes = f"LOOP_BEG notes in sched2 dump: {d.count('NOTE_INSN_LOOP_BEG')}"
    finally:
        subprocess.run("rm -f src/game/Map_UpdateDrawState.c src/game/Map_UpdateDrawState.c.*", shell=True)
    return r.stdout.splitlines()[0] if r.stdout else r.stderr[-300:], notes


if sys.argv[1:2] == ["show"]:
    print(variant(sys.argv[2], sys.argv[3]))
    sys.exit()
for a in WRAPS:
    for b in WRAPS:
        print(f"{a:13} {b:13}", *score(variant(a, b), dump=True), flush=True)
