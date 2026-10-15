#!/usr/bin/env python3
"""Write build.ninja from build/link_order.us.txt (tools/carve.py writes it), and objdiff.json beside it.

Every path in the build is relative to the repository root (ninja runs
there): the checkout may live under a directory with spaces in its name.

Each C unit also gets a target object, build/target/src/<path>.o, assembled
from splat's per-function files and data files (asm/matchings, asm/data), so
objdiff can compare it with the compiled base object.
"""
import json
import re
import struct
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
VER = "us"

EE_AS = "tools/cc/ee-gcc2.9-991111/bin/as"
LD = "tools/cc/binutils-2.10-ee/bin/ld"
OBJDIFF = "tools/cc/objdiff-cli"

OBJDIFF_SCHEMA = "https://raw.githubusercontent.com/encounter/objdiff/main/config.schema.json"
# the instruction comment of splat's function files: /* ROMOFF VRAM BYTES */
INSN = re.compile(r"/\* [0-9A-Fa-f]+ ([0-9A-Fa-f]{8}) ")
# a load from the literal pool in a function file
# the address comment of a splat data line: /* ROMOFF VRAM ... */, or /* VRAM */ in .bss
DATA_ADDR = re.compile(r"/\* ([0-9A-F]+)(?: ([0-9A-F]{8}))?(?: [0-9A-F]+)? \*/")
LIT4_LOAD = re.compile(r"\blwc1(\s+)(\$f\d+), (D_[0-9A-F]{8})$")
TARGET_HEAD = ('.include "macro.inc"\n\n.set noat\n.set noreorder\n\n'
               '.section .text, "ax"\n.align 3\n\n')


def link_order() -> list[str]:
    lines = (ROOT / f"build/link_order.{VER}.txt").read_text().splitlines()
    return [l.strip() for l in lines if l.strip() and not l.lstrip().startswith("#")]


def obj_for(src: str) -> str:
    return f"build/{src}.o"


def stem(src: str) -> str:
    return src.rsplit(".", 1)[0]


def c_parts(src: str) -> tuple[list[Path], list[Path]]:
    """The function files and data files splat wrote for a C unit."""
    name = src[len("src/"):-len(".c")]
    mdir = ROOT / "asm/matchings" / name
    funcs = sorted(mdir.glob("*.s")) if mdir.is_dir() else []
    datas = [ROOT / f"asm/data/{name}.{sec}.s"
             for sec in ("data", "rodata", "lit4", "sdata", "sbss", "bss")]
    return funcs, [d for d in datas if d.exists()]


def vram_of(path: Path) -> int:
    m = INSN.search(path.read_text())
    if not m:
        raise SystemExit(f"{path}: no instruction comment")
    return int(m.group(1), 16)


def trimmed(path: Path) -> str:
    """A splat function file up to its endlabel: the bytes after it are splat's
    padding to the next symbol's alignment, which the C object does not have. A
    file with no endlabel is kept whole."""
    lines = path.read_text().splitlines(keepends=True)
    ends = [i for i, l in enumerate(lines) if l.startswith("endlabel")]
    if ends:
        lines = lines[:ends[0] + 1]
    return "".join(lines)


def zero_bytes(body: str) -> int | None:
    """The bytes a data directive holds, if they are all zero; else None."""
    d, _, arg = body.partition(" ")
    if d == ".asciz" and arg.strip() == '""':
        return 1
    if d not in (".word", ".byte", ".space"):
        return None
    try:
        n = int(arg, 0)
    except ValueError:      # a symbol, or a label
        return None
    if d == ".space":
        return n
    return (4 if d == ".word" else 1) if n == 0 else None


def jtbl_fill(text: str) -> str:
    """Drop the zero words at the end of a jump table (jtbl_ + 8 hex).

    Splat sizes a table up to the next symbol, and the zero words before it are
    the alignment fill, not table entries (the entries are .L labels)."""
    out, block = [], None
    for l in text.splitlines(keepends=True):
        if l.startswith("dlabel jtbl_"):
            block = []
        if block is not None:
            if l.startswith("enddlabel"):
                while block and block[-1].split("*/")[-1].strip() == ".word 0x00000000":
                    block.pop()
                out += block
                block = None
            else:
                block.append(l)
                continue
        out.append(l)
    return "".join(out)


def trailing_fill(text: str) -> str:
    """The data after the last symbol's content is not the C object's.

    Splat pads each symbol with .align up to the next one, and gives the
    alignment fill before the next unit a symbol of its own. After the last
    symbol's enddlabel (or after its body, when it has none), the .align lines
    and zero words are dropped; then so is that last symbol if it is an
    auto-named one (D_ + 8 hex) that holds only zeros and is under 16 bytes.
    The bss tail splat names _end is the same fill, whatever its size."""
    lines = text.splitlines(keepends=True)
    dl = [i for i, l in enumerate(lines) if l.startswith("dlabel ")]
    if not dl:
        return text
    start = dl[-1]
    el = next((i for i in range(start, len(lines)) if lines[i].startswith("enddlabel ")), None)
    body = lambda l: l.split("*/")[-1].strip()
    floor = el + 1 if el is not None else len(lines)
    stop = len(lines)
    while stop > floor:
        b = body(lines[stop - 1])
        if b and not b.startswith(".align") and zero_bytes(b) is None:
            break
        stop -= 1
    del lines[stop:]
    end = el if el is not None else len(lines)
    while end > start + 1 and body(lines[end - 1]).startswith(".align"):
        del lines[end - 1]
        end -= 1
    name, size, zero = lines[start].split()[1], 0, True
    for l in lines[start + 1:end]:
        b = body(l)
        if not b or b.startswith(".align"):
            continue
        n = zero_bytes(b)
        if n is None:
            zero = False
            break
        size += n
    if zero and (name == "_end" or (re.fullmatch(r"D_[0-9A-F]{8}", name) and size < 16)):
        if start and lines[start - 1].startswith("nonmatching "):
            start -= 1
        return "".join(lines[:start])
    return "".join(lines)


def target_src(out: str) -> str:
    """The C unit whose target source is build/target/src/<path>.s."""
    return "src/" + out[len("build/target/src/"):-len(".s")] + ".c"


def lit4_values(path: Path) -> dict[str, str]:
    """label -> the float a splat .lit4 file holds there, written for li.s."""
    out, label = {}, None
    for l in path.read_text().splitlines():
        if l.startswith("dlabel "):
            label = l.split()[1]
        elif label and ".word" in l:
            bits = int(l.split(".word", 1)[1].split()[0], 0)
            out[label] = repr(struct.unpack("<f", struct.pack("<I", bits))[0])
            label = None
    return out


def li_s(text: str, lit4: dict[str, str]) -> str:
    """Each load of a pool entry back to the li.s gcc wrote.

    ee-as cannot load a .lit4 symbol the same file defines, and from li.s it
    builds the unit's pool as it does for the C object: one entry per load, in
    order, with the same relocations."""
    out = []
    for l in text.splitlines(keepends=True):
        if l.startswith(".extern ") and l.split()[1].rstrip(",") in lit4:
            continue
        m = LIT4_LOAD.search(l.rstrip("\n"))
        if m and m.group(3) in lit4:
            l = l[:m.start()] + f"li.s{m.group(1)}{m.group(2)}, {lit4[m.group(3)]}\n"
        out.append(l)
    return "".join(out)


def base_data_names(src: str, datas: list[Path]) -> dict[int, str] | None:
    """address -> name of each data symbol the compiled C object defines, placed
    by the start address of splat's file for that section; None before the
    object is built."""
    from elftools.elf.elffile import ELFFile
    path = ROOT / obj_for(src)
    if not path.exists():
        return None
    starts = {}
    for d in datas:
        m = DATA_ADDR.search(d.read_text())
        if m:
            starts["." + d.name.rsplit(".", 2)[-2]] = int(m.group(2) or m.group(1), 16)
    out = {}
    with open(path, "rb") as f:
        e = ELFFile(f)
        secs = [s.name for s in e.iter_sections()]
        for s in e.get_section_by_name(".symtab").iter_symbols():
            shndx = s["st_shndx"]
            sec = secs[shndx] if isinstance(shndx, int) else None
            if (sec in starts and s.name and not s.name.startswith(".L")
                    and s["st_info"]["type"] not in ("STT_SECTION", "STT_FUNC")):
                out.setdefault(starts[sec] + s["st_value"], s.name)
    return out


def name_like_base(text: str, data: str, names: dict[int, str]) -> tuple[str, str]:
    """Give the data labels the C object's names, so objdiff pairs them.

    A label at an address where the C object has a symbol takes its name (a
    static the C names, which splat calls D_XXXXXXXX). A label where the C
    object has none (a string literal, a jump table, a pointer into an array)
    becomes a local .L label, as gcc's .LC labels are."""
    lines = data.splitlines(keepends=True)
    rename = {}
    for i, l in enumerate(lines):
        if not l.startswith("dlabel "):
            continue
        label = l.split()[1]
        m = next((DATA_ADDR.search(x) for x in lines[i + 1:i + 3] if DATA_ADDR.search(x)), None)
        if not m:
            continue
        addr = int(m.group(2) or m.group(1), 16)
        new = names.get(addr, ".L" + label)
        if new != label:
            rename[label] = new
    if not rename:
        return text, data
    word = re.compile(r"(?<![\w.$])(" + "|".join(map(re.escape, rename)) + r")\b")
    out = []
    for l in data.splitlines(keepends=True):
        parts = l.split()
        if parts[:1] in (["dlabel"], ["enddlabel"], ["nonmatching"]) and \
                rename.get(parts[1].rstrip(","), "").startswith(".L"):
            if parts[0] == "dlabel":
                out.append(rename[parts[1]] + ":\n")
            continue
        out.append(word.sub(lambda m: rename[m.group(1)], l))
    text = "".join(l for l in text.splitlines(keepends=True)
                   if not (l.startswith(".extern ") and
                           rename.get(l.split()[1].rstrip(","), "").startswith(".L")))
    return word.sub(lambda m: rename[m.group(1)], text), "".join(out)


def target_text(src: str) -> str:
    """One assembly file for a C unit: its functions in address order, then its
    data. Its .lit4 comes from li.s in the functions, not from the data file."""
    funcs, datas = c_parts(src)
    text = "".join(trimmed(f) for f in sorted(funcs, key=vram_of))
    lit4 = [d for d in datas if d.name.endswith(".lit4.s")]
    if lit4:
        text = li_s(text, lit4_values(lit4[0]))
    data = ""
    for d in datas:
        if d in lit4:
            continue
        body = trailing_fill(jtbl_fill(d.read_text())).splitlines(keepends=True)
        data += "".join(l for l in body if l.strip() != '.include "macro.inc"')
    names = base_data_names(src, [d for d in datas if d not in lit4])
    if names is not None:
        text, data = name_like_base(text, data, names)
    text = TARGET_HEAD + text + data
    return text if text.endswith("\n") else text + "\n"


def write_if_changed(path: Path, text: str) -> None:
    """Rewrite only on a content change, so ninja does not rebuild needlessly."""
    path.parent.mkdir(parents=True, exist_ok=True)
    if path.exists() and path.read_text() == text:
        return
    path.write_text(text)


def categories(srcs: list[str]) -> list[str]:
    """The game's C and splat units sit between crt0 and the first library
    unit; everything from there on (library C and the asm stretches between)
    is sce. Names cannot tell them apart: game C units split the game's asm
    into text_XXXXXXXX units just as library units do."""
    lib = next(i for i, s in enumerate(srcs) if i and s.startswith("src/sce/"))
    return ["sce" if i >= lib or s.startswith("src/sce/") else "game"
            for i, s in enumerate(srcs)]


def objdiff_config(srcs: list[str]) -> dict:
    units = []
    for s, cat in zip(srcs, categories(srcs)):
        unit = {"name": s.split("/", 1)[1].rsplit(".", 1)[0],
                "metadata": {"progress_categories": [cat]}}
        if s.startswith("asm/"):
            unit["target_path"] = obj_for(s)
        elif s.startswith("src/") and s.endswith(".c"):
            unit["target_path"] = f"build/target/{stem(s)}.o"
            unit["base_path"] = obj_for(s)
            unit["metadata"]["source_path"] = s
            # linked into the matching ROM: carve stops on a unit that
            # does not match as a whole
            unit["metadata"]["complete"] = True
        else:
            # hand-written asm: the same object is both target and base
            unit["target_path"] = unit["base_path"] = obj_for(s)
            unit["metadata"]["source_path"] = s
            unit["metadata"]["complete"] = True
        units.append(unit)
    watch = [f"{root}/**/*.{ext}" for root in ("src", "include")
             for ext in ("c", "h", "s", "inc")]
    return {
        "$schema": OBJDIFF_SCHEMA,
        "custom_make": "ninja",
        "build_base": True,
        "build_target": False,
        "watch_patterns": watch,
        "progress_categories": [{"id": "game", "name": "Game"},
                                {"id": "sce", "name": "SDK and runtime libraries"}],
        "units": units,
    }


def main() -> None:
    if sys.argv[1:2] == ["--target"]:
        # the target_src rule (restat): rewrites one target source, and only on
        # a content change
        out = sys.argv[2]
        write_if_changed(ROOT / out, target_text(target_src(out)))
        return
    srcs = link_order()
    objs = [obj_for(s) for s in srcs]
    build = ROOT / "build"
    build.mkdir(exist_ok=True)
    script = (ROOT / f"config/kfiv.{VER}.ld.in").read_text()
    script = re.sub(r"@INPUTS:(\w+)@",
                    lambda m: "\n        ".join(f"{o}(.{m.group(1)})" for o in objs), script)
    (build / "kfiv.ld").write_text(script)

    c_units = [s for s in srcs if s.startswith("src/") and s.endswith(".c")]
    # one objdiff unit per link-order entry
    (ROOT / "objdiff.json").write_text(json.dumps(objdiff_config(srcs), indent=2) + "\n")

    n = []
    n.append("# Generated by tools/gen_ninja.py; do not edit.\n")
    n.append("rule as\n"
             f"  command = tools/period_env.sh {EE_AS} -EL -mcpu=5900 -mabi=eabi -G 8 -Iinclude -o $out $in\n"
             "  description = AS $in\n")
    n.append("rule cc\n"
             "  command = tools/compile_c.sh $in $out\n"
             "  description = CC $in\n")
    n.append("rule ld\n"
             f"  command = {LD} -T build/kfiv.ld -Map build/kfiv.map -o $out $in $syms\n"
             "  description = LD $out\n")
    n.append("rule rom\n"
             "  command = .venv/bin/python tools/check.py rom $in $out\n"
             "  description = ROM $out\n")
    n.append("rule check\n"
             "  command = .venv/bin/python tools/check.py verify $in && touch $out\n"
             "  description = CHECK $in\n")
    n.append("rule target_src\n"
             "  command = .venv/bin/python tools/gen_ninja.py --target $out\n"
             "  restat = 1\n"
             "  description = TARGET $out\n")
    for s, o in zip(srcs, objs):
        # src/ holds C and hand-written asm (assembled per origin by
        # compile_c.sh); asm/ is splat's output
        rule = "cc" if s.startswith("src/") else "as"
        n.append(f"build {o}: {rule} {s}" + (" | include/macro.inc" if rule == "as" else ""))
    for s in c_units:
        # the .s is rewritten only when its content changes (restat), so the
        # object is not rebuilt needlessly
        t = f"build/target/{stem(s)}.s"
        funcs, datas = c_parts(s)
        # the C object names the data labels (name_like_base)
        deps = " ".join(["tools/gen_ninja.py", obj_for(s)] + [str(p.relative_to(ROOT)) for p in funcs + datas])
        n.append(f"build {t}: target_src | {deps}")
        n.append(f"build build/target/{stem(s)}.o: as {t} | include/macro.inc")
    # splat's lists of addresses referenced but not defined yet, given to ld
    # as implicit linker scripts
    syms = "config/undefined_syms_auto.txt config/undefined_funcs_auto.txt"
    n.append(f"build build/kfiv.elf: ld {' '.join(objs)} | build/kfiv.ld {syms}\n"
             f"  syms = {syms}")
    n.append("build build/kfiv.rom: rom build/kfiv.elf")
    n.append("build build/kfiv.ok: check build/kfiv.rom")
    n.append("rule regen\n"
             "  command = .venv/bin/python tools/gen_ninja.py\n"
             "  generator = 1\n"
             "  description = REGEN build.ninja\n")
    regen_in = ["tools/gen_ninja.py", f"build/link_order.{VER}.txt",
                f"config/kfiv.{VER}.ld.in"]
    n.append(f"build build.ninja build/kfiv.ld objdiff.json: regen | {' '.join(regen_in)}")
    # objdiff's progress report over every object of the link
    all_objs = objs + [f"build/target/{stem(s)}.o" for s in c_units]
    n.append("rule report\n"
             f"  command = {OBJDIFF} report generate -o $out\n"
             "  description = REPORT $out\n")
    n.append(f"build build/report.json: report {' '.join(all_objs)} | objdiff.json {OBJDIFF}")
    # `progress` writes no file, so ninja runs it on every call and the
    # summary always prints (console pool: the output is not captured)
    n.append("rule progress\n"
             "  command = .venv/bin/python tools/progress.py --html build/progress.html\n"
             "  description = PROGRESS\n"
             "  pool = console\n")
    n.append("build progress: progress build/report.json | tools/progress.py")
    n.append("default build/kfiv.ok")
    (ROOT / "build.ninja").write_text("\n".join(n) + "\n")
    print(f"build.ninja: {len(srcs)} units, {len(c_units)} target objects")


if __name__ == "__main__":
    main()
