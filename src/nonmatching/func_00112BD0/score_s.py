"""score_s.py FILE.c... : diff count per file with the stray loop .p2align 3 removed (see try_loopwrap.py)."""
import os, sys
sys.argv, files = [sys.argv[0], "src/nonmatching/Map_UpdateDrawState.c"], sys.argv[1:]
src = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "try_loopwrap.py")).read()
src = src.split('if sys.argv[2:3] == ["show"]:')[0].replace("__file__", repr(os.path.abspath(__file__)))
exec(src)
for f in files:
    print(f"{score(open(f).read()):5} {f}", flush=True)
