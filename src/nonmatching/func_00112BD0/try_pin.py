"""Pin fog-block variables to retail's FP registers (register ... asm("$fN")) and score each via a temp copy in src/game."""
import re, subprocess, sys
BASE = open("src/nonmatching/func_00112BD0.c").read()
RETAIL = {"fade_out": "$f2", "fade_in": "$f3", "green_from": "$f4", "green_to": "$f6",
          "blue_from": "$f5", "blue_to": "$f0", "blend_sum": "$f1", "far_to": "$f9"}


def pin(src, names):
    for name in names:
        decl = re.compile(r"^(\s*)f32 %s;$" % name, re.M)
        assert len(decl.findall(src)) == 1, name
        src = decl.sub(r'\1register f32 %s asm("%s");' % (name, RETAIL[name]), src)
    return src


def score(src):
    open("src/game/func_00112BD0.c", "w").write(src)
    try:
        r = subprocess.run([".venv/bin/python", "tools/match.py", "diff", "src/game/func_00112BD0.c"], capture_output=True, text=True)
    finally:
        subprocess.run(["rm", "-f", "src/game/func_00112BD0.c"])
    out = r.stdout.splitlines()
    return out[0] if out else "compile failed: " + r.stderr.strip().splitlines()[-1]


def block(src):
    """Our fog block (66 words at function offset 0x60C) next to retail's."""
    import rabbitizer
    from elftools.elf.elffile import ELFFile
    open("src/game/func_00112BD0.c", "w").write(src)
    try:
        subprocess.run(["tools/compile_c.sh", "src/game/func_00112BD0.c", "build/permuter/pin.o"], check=True, capture_output=True)
    finally:
        subprocess.run(["rm", "-f", "src/game/func_00112BD0.c"])
    ours = ELFFile(open("build/permuter/pin.o", "rb")).get_section_by_name(".text").data()[0x60C:0x60C + 66 * 4]
    retail = open("baserom/us/SLUS_203.18", "rb").read() if False else None
    lines = []
    for k in range(0, len(ours), 4):
        w = int.from_bytes(ours[k:k + 4], "little")
        lines.append(rabbitizer.Instruction(w).disassemble())
    return lines


if sys.argv[1:2] == ["dump"]:
    names = sys.argv[2].split("+") if len(sys.argv) > 2 else []
    retail = [l.split("*/", 1)[1].strip() for l in open("/tmp/retail.s") if "/* 13" in l]
    retail = [l for l in retail][retail.index(next(l for l in retail if l.startswith("addiu      $2, $0, 0x1E"))):][:66]
    for a, b in zip(block(pin(BASE, names)), retail):
        b = " ".join(b.split()[1:]) if b[:8].isalnum() else b
        a = " ".join(a.split())
        print(("   " if a.split("#")[0].strip() == " ".join(b.split("#")[0].split()) else ">> ") + f"{a:40} | {b}")
    sys.exit()

for spec in sys.argv[1:] or ["base"]:
    names = [] if spec == "base" else spec.split("+")
    print(f"{spec:60} {score(pin(BASE, names))}")
