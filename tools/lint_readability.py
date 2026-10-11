#!/usr/bin/env python3
"""Flag game C that breaks the readability rules (CONTRIBUTING.md, "Readable C").

    tools/lint_readability.py                  summary over src/game/*.c
    tools/lint_readability.py FILE...          every finding in FILE(s)
    tools/lint_readability.py --changed [REF]  findings in src/game files added or
                                               changed since REF (default origin/main);
                                               exit 1 if there are any

Rules:
  m2c-name      m2c leftovers: arg0, temp_a0, var_v0, phi_s1, sp10
  raw-type      unsigned/short/long/signed char instead of common.h's u8..u64
  short-name    locals, parameters and fields named s, v, p, th, tmp...
                (i, j, k, n and x, y, z, w are fine)
  local-struct  a struct defined in the .c instead of a shared header
  offset-cast   raw offset access like *(int *)((char *)p + 0x10)
  pasted-types  decomp.me's typedef block or #define NULL

It is a regex scan, not a parser: treat findings as hints. Nothing here can
break the match, but fixing a finding can, so rebuild afterwards.
"""
import argparse
import os
import re
import subprocess
import sys
from collections import Counter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.path.join(ROOT, "src", "game")

TYPE = r"(?:const\s+|volatile\s+)*(?:void|char|int|float|double|[usf](?:8|16|32|64)|[A-Z]\w*)\b\s*\**"
NAME_RULES = {"m2c-name", "short-name"}  # reported once per name per file
SHORT_NAMES_OK ={"i", "j", "k", "n", "x", "y", "z", "w"}
GENERIC_NAMES = {"tmp", "val", "ret", "res", "ptr", "cnt", "f64", "buf2"}

RULES = {
    "m2c-name": re.compile(r"\b(arg\d+|temp_\w+|var_\w+|phi_\w+|sp[0-9A-F]{1,3}|saved_reg_\w+)\b"),
    "raw-type": re.compile(r"\b(unsigned|signed\s+char|short|long)\b"),
    "local-struct": re.compile(r"\btypedef\s+struct\b|^\s*struct\s+\w*\s*\{", re.M),
    "offset-cast": re.compile(r"\*\s*\(\s*[\w ]+\*+\s*\)\s*\(\s*(?:\(\s*(?:char|u8|s8)\s*\*\s*\)\s*)?\w+\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)\s*\)"),
    "pasted-types": re.compile(r"^typedef\s+[a-z ]+\s+[usf](?:8|16|32|64)\s*;|^#define\s+(?:NULL|TRUE|FALSE)\b", re.M),
}
DECLARATION = re.compile(r"^[ \t]+" + TYPE + r"\s*(\w+)\s*(?:\[[^\]]*\])?\s*(?:=[^;,]*)?((?:,\s*\**\s*\w+\s*(?:\[[^\]]*\])?\s*(?:=[^;,]*)?)*);", re.M)
DECLARATOR = re.compile(r",\s*\**\s*(\w+)")
DEFINITION = re.compile(r"^\w[\w \*]*\b\w+\s*\(([^;{)]*)\)\s*\{", re.M)
PARAMETER = re.compile(r"(\w+)\s*(?:\[[^\]]*\])?\s*$")
COMMENTS_AND_STRINGS = re.compile(r"//[^\n]*|/\*.*?\*/|\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'", re.S)


def blank_out(match):
    """Replace a comment or string with spaces, keeping newlines so line numbers hold."""
    return re.sub(r"[^\n]", " ", match.group())


def is_short_name(name):
    return (len(name) <= 2 and name not in SHORT_NAMES_OK) or name in GENERIC_NAMES


def findings(path):
    """Yield (line, rule, text) for one file."""
    code = COMMENTS_AND_STRINGS.sub(blank_out, open(path, encoding="utf-8", errors="replace").read())
    line_of = lambda offset: code.count("\n", 0, offset) + 1
    for rule, pattern in RULES.items():
        for match in pattern.finditer(code):
            yield line_of(match.start()), rule, match.group().strip()
    for match in DECLARATION.finditer(code):
        names = [match.group(1)] + DECLARATOR.findall(match.group(2))
        for name in filter(is_short_name, names):
            yield line_of(match.start()), "short-name", name
    for match in DEFINITION.finditer(code):
        for parameter in match.group(1).split(","):
            name = PARAMETER.search(parameter.strip())
            if name and name.group(1) != "void" and is_short_name(name.group(1)):
                yield line_of(match.start()), "short-name", name.group(1)


def changed_files(ref):
    base = subprocess.run(["git", "merge-base", ref, "HEAD"], cwd=ROOT, capture_output=True, text=True, check=True).stdout.strip()
    names = subprocess.run(["git", "diff", "--name-only", "--diff-filter=AMR", base, "--", "src/game/*.c", "src/game/*.h"],
                           cwd=ROOT, capture_output=True, text=True, check=True).stdout.split()
    names += subprocess.run(["git", "ls-files", "--others", "--exclude-standard", "src/game"],
                            cwd=ROOT, capture_output=True, text=True, check=True).stdout.split()
    return sorted({os.path.join(ROOT, name) for name in names if name.endswith((".c", ".h"))})


def report(paths):
    total = 0
    for path in paths:
        seen = set()
        for line, rule, text in sorted(findings(path)):
            key = (rule, text) if rule in NAME_RULES else (line, rule, text)
            if key in seen:
                continue
            seen.add(key)
            print(f"{os.path.relpath(path, ROOT)}:{line}: {rule}: {text}")
            total += 1
    return total


def summary():
    paths = sorted(os.path.join(GAME, name) for name in os.listdir(GAME) if name.endswith(".c"))
    files_per_rule, clean_files, total_lines, clean_lines = Counter(), 0, 0, 0
    for path in paths:
        lines = open(path, encoding="utf-8", errors="replace").read().count("\n")
        rules = {rule for _, rule, _ in findings(path)}
        files_per_rule.update(rules)
        total_lines += lines
        if not rules:
            clean_files += 1
            clean_lines += lines
    print(f"{len(paths)} game files, {clean_files} clean ({100 * clean_files / len(paths):.1f}%), "
          f"{100 * clean_lines / total_lines:.1f}% of lines")
    for rule in [*RULES, "short-name"]:
        count = files_per_rule[rule]
        print(f"  {rule:13} {count:5} files ({100 * count / len(paths):4.1f}%)")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("files", nargs="*", help="files to check in full")
    parser.add_argument("--changed", nargs="?", const="origin/main", metavar="REF",
                        help="check src/game files changed since REF (default origin/main)")
    args = parser.parse_args()
    if args.changed:
        paths = changed_files(args.changed)
        count = report(paths)
        print(f"{count} finding(s) in {len(paths)} changed file(s)", file=sys.stderr)
        sys.exit(1 if count else 0)
    if args.files:
        report([os.path.abspath(path) for path in args.files])
    else:
        summary()


if __name__ == "__main__":
    main()
