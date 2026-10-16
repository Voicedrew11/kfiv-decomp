#!/usr/bin/env python3
"""Check names across the whole tree: config/symbol_addrs.txt against the game C.

    tools/lint_symbols.py                  every check below; exit 1 on any finding
    tools/lint_symbols.py --changed [REF]  also reject generated or disc-derived files
                                           added or changed since REF (default origin/main)

Checks:
  duplicate-name   a name given twice in config/symbol_addrs.txt
  game-order       the game section of symbol_addrs.txt out of address order
  duplicate-addr   two names for one address in the game section
  stale-name       C still says func_/D_XXXXXXXX where symbol_addrs.txt names the address
  unknown-name     a Module_Verb function used in C that symbol_addrs.txt doesn't name
                   (and no file defines static)
  function-order   a file's functions out of address order (gcc emits them as written)
  forbidden-path   (--changed) a generated or disc-derived file in the change

Needs nothing but the checkout, so CI runs it without the disc.
"""
import argparse
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from lint_readability import GAME, ROOT, game_files as game_paths, read_code  # noqa: E402

SYMBOL_ADDRS = os.path.join(ROOT, "config", "symbol_addrs.txt")
SYMBOL = re.compile(r"^\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+)\s*;(.*)$")
SPLAT_NAME = re.compile(r"\b(?:func|D)_([0-9A-F]{8})\b")
GAME_CALL = re.compile(r"\b((?:KF_)?[A-Z][a-z][A-Za-z0-9]*_[A-Z]\w*)\s*\(")
STATIC_GAME = re.compile(r"\bstatic\b[^;{(]*\b((?:KF_)?[A-Z][a-z][A-Za-z0-9]*_[A-Z]\w*)\s*\(")
DEFINITION = re.compile(r"^(?:static\s+)?(?:inline\s+)?[A-Za-z_][\w \t*]*?\b(\w+)\s*\([^;{]*\)\s*\{", re.M)
FORBIDDEN = re.compile(r"^(?:asm/|build/|baserom/|tools/cc/|\.venv/|build\.ninja$|objdiff\.json$|config/undefined_)"
                       r"|\.(?:iso|elf|rom|bin)$", re.I)


def symbols():
    """(line, name, address, is_function, in_game_section) for each entry of symbol_addrs.txt."""
    in_game = False
    for number, line in enumerate(open(SYMBOL_ADDRS, encoding="utf-8"), 1):
        if line.startswith("// game:"):
            in_game = True
        match = SYMBOL.match(line)
        if match:
            yield number, match.group(1), int(match.group(2), 16), "type:func" in match.group(3), in_game


def game_files():
    return game_paths(".c", ".h")


def line_of(code, offset):
    return code.count("\n", 0, offset) + 1


def symbol_findings():
    where = os.path.relpath(SYMBOL_ADDRS, ROOT)
    seen, game_addresses, previous = {}, {}, None
    for line, name, address, _, in_game in symbols():
        if name in seen:
            yield where, line, "duplicate-name", f"{name} (also line {seen[name]})"
        seen[name] = line
        if not in_game:
            continue
        if previous and address < previous[1]:
            yield where, line, "game-order", f"{name} = 0x{address:08X} after {previous[0]} = 0x{previous[1]:08X}"
        previous = (name, address)
        if address in game_addresses:
            yield where, line, "duplicate-addr", f"{name} and {game_addresses[address]} = 0x{address:08X}"
        game_addresses[address] = name


def source_findings():
    names, statics = {}, set()
    for _, name, address, _, _ in symbols():
        names.setdefault(name, address)
    by_address = {}
    for _, name, address, _, in_game in symbols():
        if in_game:
            by_address[address] = name
    sources = [(path, read_code(path)[1]) for path in game_files()]
    for _, code in sources:
        statics.update(STATIC_GAME.findall(code))
    for path, code in sources:
        where = os.path.relpath(path, ROOT)
        for match in SPLAT_NAME.finditer(code):
            real = by_address.get(int(match.group(1), 16))
            if real and real != match.group():
                yield where, line_of(code, match.start()), "stale-name", f"{match.group()} is {real}"
        reported = set()
        for match in GAME_CALL.finditer(code):
            name = match.group(1)
            if name not in names and name not in statics and name not in reported:
                reported.add(name)
                yield where, line_of(code, match.start()), "unknown-name", f"{name} is not in config/symbol_addrs.txt"
        if path.endswith(".c"):
            previous = None
            for match in DEFINITION.finditer(code):
                name = match.group(1)
                splat = re.fullmatch(r"func_([0-9A-F]{8})", name)
                address = int(splat.group(1), 16) if splat else names.get(name)
                if address is None:
                    continue
                if previous and address < previous[1]:
                    yield where, line_of(code, match.start()), "function-order", \
                        f"{name} (0x{address:08X}) after {previous[0]} (0x{previous[1]:08X})"
                previous = (name, address)


def path_findings(ref):
    git = lambda *args: subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True, check=True).stdout
    base = git("merge-base", ref, "HEAD").strip()
    changed = set(git("diff", "--name-only", "--diff-filter=AMR", base).split())
    changed.update(git("ls-files", "--others", "--exclude-standard").split())
    for name in sorted(changed):
        if FORBIDDEN.search(name):
            yield name, 1, "forbidden-path", "generated or disc-derived files are never committed"


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--changed", nargs="?", const="origin/main", metavar="REF",
                        help="also check the files changed since REF (default origin/main)")
    args = parser.parse_args()
    findings = list(symbol_findings()) + list(source_findings())
    if args.changed:
        findings += list(path_findings(args.changed))
    for where, line, rule, text in findings:
        print(f"{where}:{line}: {rule}: {text}")
    print(f"{len(findings)} finding(s)", file=sys.stderr)
    sys.exit(1 if findings else 0)


if __name__ == "__main__":
    main()
