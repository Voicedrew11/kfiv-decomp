"""Score whole-block static inline helper variants of the fog blend."""
import re, subprocess, sys

SRC = "src/nonmatching/Map_UpdateDrawState.c"
TMP = "src/game/Map_UpdateDrawState.c"
text = open(SRC).read()
start = text.index("            { /* fog blend")
end = text.index("        } else {\n            D_0038A1B0.unkA0 = D_0038A1B0.unk160;")
anchor = "void Map_UpdateDrawState(void) {"

CH = """    FOG_COLOR(cur)->red = (int)(FOG_COLOR(from)->red * fade_out + FOG_COLOR(to)->red * fade_in);
    FOG_COLOR(cur)->green = (int)(FOG_COLOR(from)->green * fade_out + FOG_COLOR(to)->green * fade_in);
    FOG_COLOR(cur)->blue = (int)(FOG_COLOR(from)->blue * fade_out + FOG_COLOR(to)->blue * fade_in);
"""
D0C = "    cur->unk0C = from->unk0C * fade_out + to->unk0C * fade_in;\n"
D08 = "    cur->unk08 = from->unk08 * fade_out + to->unk08 * fade_in;\n"
SIG = "static inline void BlendFog(MapSlotUnk0CValues *cur, MapSlotUnk0CValues *from, MapSlotUnk0CValues *to, f32 fade_out, f32 fade_in)"
FADES = """                fade_out = D_0038A1B0.unk9A;
                fade_in = 30 - D_0038A1B0.unk9A;
                fade_out /= 30.0f;
                fade_in /= 30.0f;
"""
CALL = "                BlendFog(&D_0038A1B0.unkA0, &D_0038A1B0.unk140, &D_0038A1B0.unk160, fade_out, fade_in);\n"

SIG_N = "static inline void BlendFog(MapSlotUnk0CValues *cur, MapSlotUnk0CValues *from, MapSlotUnk0CValues *to, int frames)"
FADES_IN = """    f32 fade_out = frames;
    f32 fade_in = 30 - frames;
    fade_out /= 30.0f;
    fade_in /= 30.0f;
"""
CALL_N = "                BlendFog(&D_0038A1B0.unkA0, &D_0038A1B0.unk140, &D_0038A1B0.unk160, D_0038A1B0.unk9A);\n"

BYTE = "static inline int BlendByte(u8 from, u8 to, f32 fade_out, f32 fade_in) { return from * fade_out + to * fade_in; }\n"
FLT = "static inline f32 BlendF(f32 from, f32 to, f32 fade_out, f32 fade_in) { return from * fade_out + to * fade_in; }\n"
CH_B = """                FOG_COLOR(&D_0038A1B0.unkA0)->red = BlendByte(FOG_COLOR(&D_0038A1B0.unk140)->red, FOG_COLOR(&D_0038A1B0.unk160)->red, fade_out, fade_in);
                FOG_COLOR(&D_0038A1B0.unkA0)->green = BlendByte(FOG_COLOR(&D_0038A1B0.unk140)->green, FOG_COLOR(&D_0038A1B0.unk160)->green, fade_out, fade_in);
                FOG_COLOR(&D_0038A1B0.unkA0)->blue = BlendByte(FOG_COLOR(&D_0038A1B0.unk140)->blue, FOG_COLOR(&D_0038A1B0.unk160)->blue, fade_out, fade_in);
"""
F_0C = "                D_0038A1B0.unkA0.unk0C = BlendF(D_0038A1B0.unk140.unk0C, D_0038A1B0.unk160.unk0C, fade_out, fade_in);\n"
F_08 = "                D_0038A1B0.unkA0.unk08 = BlendF(D_0038A1B0.unk140.unk08, D_0038A1B0.unk160.unk08, fade_out, fade_in);\n"

variants = {
    "helper ch,0C,08": (SIG + " {\n" + CH + D0C + D08 + "}\n", FADES + CALL),
    "helper ch,08,0C": (SIG + " {\n" + CH + D08 + D0C + "}\n", FADES + CALL),
    "helper 0C,08,ch": (SIG + " {\n" + D0C + D08 + CH + "}\n", FADES + CALL),
    "helper(frames) ch,0C,08": (SIG_N + " {\n" + FADES_IN + CH + D0C + D08 + "}\n", CALL_N),
    "helper(frames) ch,08,0C": (SIG_N + " {\n" + FADES_IN + CH + D08 + D0C + "}\n", CALL_N),
    "per-channel ch,0C,08": (BYTE + FLT, FADES + CH_B + F_0C + F_08),
    "per-channel ch,08,0C": (BYTE + FLT, FADES + CH_B + F_08 + F_0C),
}


def score(src):
    open(TMP, "w").write(src)
    try:
        r = subprocess.run([".venv/bin/python", "tools/match.py", "diff", TMP], capture_output=True, text=True)
    finally:
        subprocess.run(["rm", "-f", TMP])
    first = r.stdout.splitlines()[:1]
    if first and "differ" in first[0]:
        return first[0]
    return (r.stdout + r.stderr)[-400:]


for name, (helper, body) in variants.items():
    src = text[:start] + "            {\n" + body + "            }\n" + text[end:]
    src = src.replace(anchor, helper + "\n" + anchor)
    print(f"{name:28} {score(src)}", flush=True)
