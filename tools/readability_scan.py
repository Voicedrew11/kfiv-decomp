#!/usr/bin/env python3
"""Readability measures of the game C, beyond named functions.

    tools/readability_scan.py [-o build/readability.json] [-v]

Struct layouts: compiles every .c under src/game with -g and reads each struct and
union defined in src/game/ from ee-gcc's stabs, which give every field's exact
bit offset and size. A field is unknown while it is still named unkXX; padXX
(or pad) is padding, bytes no game code touches, counted apart and not as a
field. A struct is recovered once no field is unknown. A field whose type is another
named game struct is left to that struct, so each field is counted once;
anonymous nested structs are counted inside their parent. Fields are counted,
not bytes: a few large named buffers would swamp a byte measure.

Source text (comments and strings stripped):
  field uses      `->x` / `.x` accesses whose field is not unkXX
  bit operations  `& | ^` (and `&= |= ^=`) with a constant, plus `(x >> N) & 1`
                  tests; named when the constant is an UPPER_CASE name. Low-bit
                  masks (0xFF, 0xFFFF, ...) are truncation, not flags: skipped.
  debt counts     function-pointer casts, `// match:` comments, gotos, block
                  comments over one line (AGENTS.md wants one line), unkXX names
                  in comments (mostly stale: the field has a name now), `case N:`
                  labels and state/mode/kind fields set or compared to a number
                  (enum candidates), casts to a plain pointer type (`(u8 *)`,
                  `(char *)`, ...), parameters named `unknown`/`argN`, and float
                  literals with four or more decimals (`0.5235988f`: derived
                  constants such as angles in radians) outside brace
                  initializers (data tables) and #define lines. Local declarations:
                  top-level `extern`s and non-static prototypes in a .c file,
                  which belong in the module headers; those for a symbol some
                  header already declares are counted apart too.

Globals: data symbols of build/kfiv.elf not defined by an src/sce object;
named once no longer splat's D_XXXXXXXX.

Writes build/readability.json, read by tools/progress.py.
"""
import argparse
import concurrent.futures
import json
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GAME = ROOT / "src" / "game"
CC_DIR = ROOT / "tools" / "cc" / "ee-gcc2.9-991111"
SCE_OBJS = ROOT / "build" / "src" / "sce"
ELF = ROOT / "build" / "kfiv.elf"
AUTO_DATA = re.compile(r"(D|jtbl)_[0-9A-Fa-f]{8}$")
UNKNOWN = re.compile(r"unk(_?[0-9A-Fa-f]+)?(_\w*)?$")
PADDING = re.compile(r"pad(_?[0-9A-F]+)?(_\d+)?$")
STABS = re.compile(r'^\s*\.stabs\s+"((?:[^"\\]|\\.)*)"')


def game_type_names() -> set:
    """Struct and union names (tag or typedef) defined in src/game sources."""
    names = set()
    for path in sorted(GAME.rglob("*.[ch]")):
        text = re.sub(r"/\*.*?\*/|//[^\n]*", "", path.read_text(errors="replace"), flags=re.S)
        names.update(re.findall(r"\b(?:struct|union)\s+(\w+)\s*\{", text))
        for head in re.finditer(r"\btypedef\s+(?:struct|union)\s*\w*\s*\{", text):
            depth, i = 0, head.end() - 1
            for j in range(i, len(text)):
                depth += {"{": 1, "}": -1}.get(text[j], 0)
                if depth == 0:
                    m = re.match(r"\s*(\w+)\s*;", text[j + 1:])
                    if m:
                        names.add(m.group(1))
                    break
    return names


def compile_stabs(src: Path, out_dir: Path) -> str:
    incs = [f"-I{ROOT / 'include'}"] + [f"-I{d}" for d in sorted((ROOT / "src" / "sce").iterdir())
                                         if d.is_dir()] + [f"-I{GAME}"]
    out = out_dir / (src.stem + ".s")
    cmd = [str(ROOT / "tools" / "period_env.sh"), str(CC_DIR / "ee-gcc"),
           "-B", str(CC_DIR / "gcc-lib" / "ee" / "2.9-ee-991111-01") + "/",
           "-S", "-O0", "-g", "-mips3", "-EL", "-nostdinc", "-G", "8", *incs,
           "-o", str(out), src.name]
    r = subprocess.run(cmd, cwd=src.parent, capture_output=True, text=True)
    if r.returncode != 0:
        raise RuntimeError(f"{src.name}: {r.stderr.strip()[:500]}")
    return out.read_text(errors="replace")


def stab_strings(asm: str):
    """Stab strings, with gcc's backslash continuations joined."""
    pending = ""
    for line in asm.splitlines():
        m = STABS.match(line)
        if not m:
            continue
        s = m.group(1).replace('\\"', '"')
        if s.endswith("\\\\"):
            pending += s[:-2]
            continue
        yield pending + s
        pending = ""


class Parser:
    """Just enough of the stabs type grammar to walk struct fields."""

    def __init__(self, types: dict):
        self.types = types  # type number -> definition

    def parse_type(self, s: str, i: int):
        m = re.match(r"\(?(-?\d+)(?:,(\d+)\))?", s[i:])
        if m:
            num = m.group(0)
            i += len(num)
            if i < len(s) and s[i] == "=":
                d, i = self.parse_def(s, i + 1)
                self.types[num] = d
                return d, i
            return ("ref", num), i
        return self.parse_def(s, i)

    def parse_def(self, s: str, i: int):
        c = s[i]
        if c.isdigit() or c in "(-":  # alias of another type
            return self.parse_type(s, i)
        if c == "@":  # attribute, e.g. @s64;
            i = s.index(";", i) + 1
            return self.parse_type(s, i)
        if c in "*&fkBv":
            t, i = self.parse_type(s, i + 1)
            return ("ptr" if c == "*" else "other", t), i
        if c == "a":  # ar<index type>;lo;hi;<element type>
            _, i = self.parse_type(s, i + 2)
            i = s.index(";", s.index(";", i + 1) + 1) + 1
            t, i = self.parse_type(s, i)
            return ("array", t), i
        if c == "r":
            _, i = self.parse_type(s, i + 1)
            i = s.index(";", s.index(";", i + 1) + 1) + 1
            return ("scalar",), i
        if c == "e":
            return ("scalar",), s.index(";", i) + 1
        if c == "x":
            return ("xref", s[i + 2:s.index(":", i)]), s.index(":", i) + 1
        if c in "su":
            m = re.match(r"\d+", s[i + 1:])
            i += 1 + len(m.group(0))
            fields = []
            while s[i] != ";":
                colon = s.index(":", i)
                name = s[i:colon]
                t, i = self.parse_type(s, colon + 1)
                m = re.match(r",(-?\d+),(\d+);", s[i:])
                fields.append((name, t, int(m.group(1)), int(m.group(2))))
                i += len(m.group(0))
            return ("struct", None, fields), i + 1
        raise ValueError(f"unknown stabs type {s[i:i + 20]!r}")


def scan_file(asm: str, wanted: set) -> dict:
    """name -> list of (field path, known, bits) for wanted structs in one file."""
    types, named = {}, {}
    p = Parser(types)
    for s in stab_strings(asm):
        m = re.match(r"([^:]*):([tT])", s)
        if not m or not m.group(1):
            continue
        try:
            d, _ = p.parse_type(s, m.end())
        except (ValueError, AttributeError, IndexError):
            continue
        if m.group(1) in wanted and d[0] in ("struct", "ref"):
            named.setdefault(m.group(1), d)
    # numbers of named game structs, so their bytes stay with them
    owner = {}
    for name, d in named.items():
        owner[id(resolve(d, types))] = name

    def leaves(d, prefix):
        for fname, t, off, bits in d[2]:
            base = t
            while base[0] in ("ref", "array"):
                nxt = resolve(base, types) if base[0] == "ref" else base[1]
                if nxt is base:
                    break
                base = nxt
            if base[0] == "xref":
                base = resolve(named.get(base[1], base), types)
            if base[0] == "struct" and id(base) in owner:
                continue
            if base[0] == "struct" and t[0] != "array":
                yield from leaves(base, prefix + fname + ".")
                continue
            kind = "padding" if PADDING.match(fname) else "unknown" if UNKNOWN.match(fname) else "named"
            yield prefix + fname, kind, bits

    out = {}
    for name, d in named.items():
        d = resolve(d, types)
        if d[0] == "struct":
            out[name] = list(leaves(d, ""))
    return out


def resolve(d, types):
    seen = 0
    while d[0] == "ref" and d[1] in types and seen < 50:
        d, seen = types[d[1]], seen + 1
    return d


def strip_code(text: str) -> str:
    """Source with comments and string/char literals blanked."""
    return re.sub(r'/\*.*?\*/|//[^\n]*|"(?:[^"\\\n]|\\.)*"|\'(?:[^\'\\\n]|\\.)*\'',
                  lambda m: " " if m.group(0)[0] == "/" else '""', text, flags=re.S)


# binary only: the operator follows an operand, so `&x` (address-of) is not one
BIT_OP = r"(?<=[\w)\]])\s*(?<![&|])(?:[&|^]=?)(?![&|=])\s*~?\s*\(?\s*"
RAW_BITS = re.compile(BIT_OP + r"(0x[0-9A-Fa-f]+|\d+)\b(?![.\w])")
NAMED_BITS = re.compile(BIT_OP + r"(?!D_[0-9A-F]{8}\b)([A-Z][A-Z0-9_]*[A-Z0-9])\b(?!\s*\()")
SHIFT_TEST = re.compile(r">>\s*\d+\s*\)\s*&\s*1\b")
COMMENTS = re.compile(r"/\*.*?\*/|//[^\n]*", re.S)
COMMENT_UNK = re.compile(r"\bunk[0-9A-Fa-f]{2,}(?:_\w+)?\b")
RAW_CASE = re.compile(r"\bcase\s+-?(?:0x[0-9A-Fa-f]+|\d+)\s*(?::|\.\.\.)")
STATE_FIELD = r"(?:->|\.)\s*\w*(?:state|mode|kind|step|choice|anim\w*|motion_\w*)\b"
RAW_STATE = re.compile(STATE_FIELD + r"\s*(?:==|!=|=(?!=))\s*-?(?:0x[0-9A-Fa-f]+|\d+)\b(?![.\w])")
PLAIN_TYPES = r"(?:const\s+)?(?:unsigned\s+|signed\s+)?(?:char|short|int|long|float|double|void|[us](?:8|16|32|64)|f32|f64|u_long128|u_char|u_short|u_int|u_long)"
PTR_CAST = re.compile(r"\(\s*" + PLAIN_TYPES + r"\s*\*+\s*\)")
FUNC_DEF = re.compile(r"^\w[\w\s\*]*\b\w+\s*\(([^;{}]*)\)\s*\{", re.M)
PLACEHOLDER_PARAM = re.compile(r"\b(?:unknown\w*|arg_?\d+)\s*(?:\[[^\]]*\]\s*)*(?=,|$)")
PROTOTYPE = re.compile(r"^[A-Za-z_][\w\s\*]*?\b(\w+)\s*\(")
EXTERN = re.compile(r"^extern\b[^(\[]*?\b(\w+)\s*(?:\[[^\]]*\]\s*)*$|^extern\b.*?\(\s*\*\s*(\w+)\s*\)")


def top_level_statements(code: str):
    """Statements at file scope that end in `;`, preprocessor lines left out."""
    code = re.sub(r"^\s*#.*$", "", code, flags=re.M)
    depth, start = 0, 0
    for i, ch in enumerate(code):
        if ch in "{(":
            depth += 1
        elif ch in "})":
            depth -= 1
            if depth == 0 and ch == "}":
                start = i + 1
        elif ch == ";" and depth == 0:
            yield " ".join(code[start:i].split())
            start = i + 1


def local_declarations(code: str) -> list:
    """Names declared by top-level externs and non-static prototypes."""
    names = []
    for st in top_level_statements(code):
        if st.startswith(("static", "typedef")) or "=" in st:
            continue
        if st.startswith("extern"):
            m = EXTERN.match(st)
            if m:
                names.append(m.group(1) or m.group(2))
        elif (m := PROTOTYPE.match(st)) and not re.match(r"^(?:struct|union|enum)\s+\w+\s*$", st):
            names.append(m.group(1))
    return names


RAW_FLOAT = re.compile(r"(?<![\w.])\d+\.\d{4,}(?:[eE][-+]?\d+)?[fF]?\b")


def without_initializers(code: str) -> str:
    """Code with brace initializers (`= { ... }`, data tables) and #define lines (a name's value) blanked."""
    out, pos = [], 0
    for m in re.finditer(r"=\s*\{", code):
        if m.start() < pos:
            continue
        depth, i = 0, m.end() - 1
        while i < len(code):
            depth += {"{": 1, "}": -1}.get(code[i], 0)
            if depth == 0:
                break
            i += 1
        out.append(code[pos:m.start()]); pos = i + 1
    out.append(code[pos:])
    return re.sub(r"^[ \t]*#[ \t]*define\b.*", " ", " ".join(out), flags=re.M)


def source_measures(sources) -> dict:
    m = dict.fromkeys(("field_uses", "named_field_uses", "bit_ops", "named_bit_ops",
                       "fn_ptr_casts", "match_comments", "gotos", "long_comments",
                       "comment_unk_names", "raw_case_labels", "raw_state_values",
                       "plain_pointer_casts", "placeholder_params", "raw_float_constants",
                       "local_declarations", "local_declarations_in_headers"), 0)
    header_names = set()
    for h in [*GAME.rglob("*.h"), *(ROOT / "include").rglob("*.h"), *(ROOT / "src" / "sce").rglob("*.h")]:
        header_names |= set(local_declarations(strip_code(h.read_text(errors="replace"))))
    for path in sources:
        raw = path.read_text(errors="replace")
        code = strip_code(raw)
        for f in re.findall(r"(?:->|(?<![\w.)\]]\d)\.)\s*([A-Za-z_]\w*)", code):
            m["field_uses"] += 1
            m["named_field_uses"] += not UNKNOWN.match(f)
        shift = len(SHIFT_TEST.findall(code))
        code_no_shift = SHIFT_TEST.sub(" ", code)
        raw_bits = [v for v in RAW_BITS.findall(code_no_shift)
                    if (n := int(v, 0)) and n & (n + 1)]  # 2^k - 1 is a truncation mask
        named_bits = NAMED_BITS.findall(code_no_shift)
        m["bit_ops"] += shift + len(raw_bits) + len(named_bits)
        m["named_bit_ops"] += len(named_bits)
        m["fn_ptr_casts"] += len(re.findall(r"\(\s*\w[\w\s*]*\(\s*\*\s*\)\s*\([^()]*\)\s*\)", code))
        m["match_comments"] += len(re.findall(r"//\s*match:", raw))
        m["gotos"] += len(re.findall(r"\bgoto\b", code))
        m["long_comments"] += sum(1 for c in re.findall(r"/\*.*?\*/", raw, re.S) if "\n" in c)
        m["comment_unk_names"] += sum(len(COMMENT_UNK.findall(c)) for c in COMMENTS.findall(raw))
        m["raw_case_labels"] += len(RAW_CASE.findall(code))
        m["raw_state_values"] += len(RAW_STATE.findall(code))
        m["plain_pointer_casts"] += len(PTR_CAST.findall(code))
        m["placeholder_params"] += sum(len(PLACEHOLDER_PARAM.findall(p.replace("\n", " ").strip()))
                                       for d in FUNC_DEF.findall(code) for p in d.split(","))
        m["raw_float_constants"] += len(RAW_FLOAT.findall(without_initializers(code)))
        local = local_declarations(code)
        m["local_declarations"] += len(local)
        m["local_declarations_in_headers"] += sum(1 for n in local if n in header_names)
    return m


def defined_symbols(obj: Path) -> set:
    r = subprocess.run(["nm", "--defined-only", str(obj)], capture_output=True, text=True)
    return {line.split()[-1] for line in r.stdout.splitlines() if len(line.split()) == 3}


def global_measures() -> dict:
    """Game data symbols in the linked ELF: all of them, and those named."""
    if not ELF.exists():
        return {}
    sce = set()
    for obj in SCE_OBJS.rglob("*.o"):
        sce |= defined_symbols(obj)
    r = subprocess.run(["nm", "--defined-only", str(ELF)], capture_output=True, text=True)
    names = [parts[2] for parts in (l.split() for l in r.stdout.splitlines())
             if len(parts) == 3 and parts[1] in "dDbBrRgGsSV"
             and not parts[2].startswith((".", "$", "_gp")) and parts[2] not in sce]
    return {"total_globals": len(names),
            "named_globals": sum(1 for n in names if not AUTO_DATA.match(n))}


def struct_rows(sources) -> list:
    wanted = game_type_names()
    structs = {}
    with tempfile.TemporaryDirectory() as tmp, \
            concurrent.futures.ThreadPoolExecutor(os.cpu_count() or 4) as pool:
        for src, asm in zip(sources, pool.map(lambda s: compile_stabs(s, Path(tmp)), sources)):
            for name, fields in scan_file(asm, wanted).items():
                # one layout per name; static structs that share a name get the file
                key = name if structs.get(name, (fields,))[0] == fields else f"{name}@{src.name}"
                structs.setdefault(key, (fields, src.name))
    rows = []
    for name, (fields, _) in sorted(structs.items()):
        kinds = [k for _, k, _ in fields]
        if kinds.count("padding") < len(kinds):
            rows.append({"name": name, "fields": len(kinds) - kinds.count("padding"),
                         "named_fields": kinds.count("named"), "padding_fields": kinds.count("padding")})
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("-o", "--out", default=str(ROOT / "build" / "readability.json"))
    ap.add_argument("-v", "--verbose", action="store_true", help="list every struct")
    a = ap.parse_args()

    sources = sorted(GAME.rglob("*.c"))
    rows = struct_rows(sources)
    measures = {
        "total_structs": len(rows),
        "recovered_structs": sum(1 for r in rows if r["named_fields"] == r["fields"]),
        "total_fields": sum(r["fields"] for r in rows),
        "named_fields": sum(r["named_fields"] for r in rows),
        "padding_fields": sum(r["padding_fields"] for r in rows),
        **source_measures(sources),
        **global_measures(),
    }
    out = Path(a.out)
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps({"measures": measures, "structs": rows}, indent=1) + "\n")
    if a.verbose:
        for r in sorted(rows, key=lambda r: r["fields"] - r["named_fields"], reverse=True):
            print(f"{r['name']:40} {r['named_fields']:4}/{r['fields']} fields named")
    print(f"wrote {out.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
