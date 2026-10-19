"""diff_s.py SRC.c 'python-expr-editing-s' : compile SRC (copied into src/game) to .s, edit, assemble, diff vs retail.

The edit expression sees the assembly text as `s` and returns the new text.
"""
import os, subprocess, sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../.."))
sys.path.insert(0, os.path.join(ROOT, "tools"))
os.chdir(ROOT)
import match

src, edit = sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else "s"
tmp = "src/game/Map_UpdateDrawState.c"
open(tmp, "w").write(open(src).read())
out = "build/match/src/game/Map_UpdateDrawState.c.o"
try:
    subprocess.run(["tools/compile_c.sh", tmp, out], check=True)
    s_path = out[:-2] + ".s"
    s = open(s_path).read()
    s = eval(edit)
    open(s_path, "w").write(s)
    subprocess.run(["tools/period_env.sh", "build/ee-as-2.9-fill", "-EL", "-mcpu=5900", "-mabi=eabi", "-G", "8",
                    "-o", out, s_path], check=True)
    real_run = subprocess.run
    subprocess.run = lambda *a, **k: subprocess.CompletedProcess(a, 0)
    match.diff(tmp)
    subprocess.run = real_run
finally:
    os.remove(tmp)
