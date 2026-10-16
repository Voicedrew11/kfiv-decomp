#!/usr/bin/env python3
"""Flag game C that breaks the readability rules (CONTRIBUTING.md, "Readable C").

    tools/lint_readability.py                  summary over the .c files under src/game
    tools/lint_readability.py FILE...          every finding in FILE(s)
    tools/lint_readability.py --changed [REF]  findings in src/game files added since REF
                                               (default origin/main), and on the lines
                                               changed in other files; exit 1 if any

Rules:
  m2c-name        m2c leftovers: arg0, temp_a0, var_v0, phi_s1, sp10
  raw-type        unsigned/short/long/signed char instead of common.h's u8..u64
  short-name      locals, parameters and fields named s, v, p, th, tmp...
                  (i, j, k, n and x, y, z, w are fine)
  hex-name        offset-style names like f1B4, s44, p0C; unknown fields are unk1B4
  match-artifact  permuter names: new_var, dummy
  local-struct    a struct defined in the .c instead of a shared header
  generic-type    a local type named Obj, Ctl, Actor, Sub, A... (name it for what it is)
  offset-cast     raw offset access like *(int *)((char *)p + 0x10)
  hw-define       a local #define for an EE register; use eeregs.h (GIF_STAT, D1_CHCR...)
  compound-assign x = x + y instead of x += y
  missing-common  no #include "common.h" (directly or via a game header)
  proto-conflict  a function declared differently from its definition, or from
                  the most common declaration when it is still asm
  pasted-types    decomp.me's typedef block or #define NULL
  inline-address  a raw address in code: (int *)0x70000010, (u32 *)0x1000B000, or a bare
                  scratchpad 0x7000xxxx; name it once with a #define in a shared header
  data-conflict   a global declared as a different scalar type, or as a pointer, scalar
                  or aggregate, than its definition or most of its declarations say

A "// match:" comment saying why the code has to look that way suppresses the
findings on its line, or on the next line when it stands on a line of its own.

It is a regex scan, not a parser: treat findings as hints. Nothing here can
break the match, but fixing a finding can, so rebuild afterwards.
"""
import argparse
import os
import re
import subprocess
import sys
from collections import Counter, defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.path.join(ROOT, "src", "game")


def game_files(*suffixes):
    """The paths of the files under src/game (all its folders) ending in one of SUFFIXES."""
    return sorted(os.path.join(root, name) for root, _, names in os.walk(GAME)
                  for name in names if name.endswith(suffixes))

TYPE = r"(?:const\s+|volatile\s+)*(?:void|char|int|float|double|[usf](?:8|16|32|64)|[A-Z]\w*)\b\s*\**"
NAME_RULES = {"m2c-name", "short-name", "hex-name", "match-artifact", "generic-type"}  # reported once per name per file
SHORT_NAMES_OK = {"i", "j", "k", "n", "x", "y", "z", "w"}
GENERIC_NAMES = {"tmp", "val", "ret", "res", "ptr", "cnt", "f64", "buf2"}
GENERIC_TYPES = {"Actor", "Arg", "Args", "Base", "Buf", "Ctl", "Ctx", "Data", "Dev", "Elem", "Ent", "Entry",
                 "Flags", "Hdr", "Header", "Info", "Inner", "Item", "Mgr", "Mid", "Node", "Obj", "Outer",
                 "Owner", "Param", "Params", "Rec", "Row", "State", "Struct", "Sub", "Table", "Tbl",
                 "Thing", "Unk", "Work"}
TYPE_WORDS = {"void", "char", "int", "float", "double", "short", "long", "signed", "unsigned",
              "const", "volatile", "struct", "union", "enum", "s8", "s16", "s32", "s64", "u8", "u16",
              "u32", "u64", "f32", "f64"}

RULES = {
    "m2c-name": re.compile(r"\b(arg\d+|temp_\w+|var_\w+|phi_\w+|sp[0-9A-F]{1,3}|saved_reg_\w+)\b"),
    "raw-type": re.compile(r"\b(unsigned|signed\s+char|short|long)\b"),
    "hex-name": re.compile(r"\b(?![usf](?:8|16|32|64)\b)[a-z](?=[0-9A-F]*\d)[0-9A-F]{2,4}\b"),
    "match-artifact": re.compile(r"\b(?:new_var|dummy)\w*\b"),
    "local-struct": re.compile(r"\btypedef\s+struct\b|^\s*struct\s+\w*\s*\{", re.M),
    "offset-cast": re.compile(r"\*\s*\(\s*[\w ]+\*+\s*\)\s*\(\s*(?:\(\s*(?:char|u8|s8)\s*\*\s*\)\s*)?\w+\s*\+\s*(?:0x[0-9A-Fa-f]+|\d+)\s*\)"),
    "hw-define": re.compile(r"^#define\s+(?P<text>\w+)\s+\(\s*\*\s*\(\s*(?:volatile\s+)?[\w ]+\*\s*\)\s*0x1[0-9A-Fa-f]{7}\s*\)", re.M),
    "compound-assign": re.compile(r"(?<![\w.\]>])(?P<lhs>[A-Za-z_]\w*(?:(?:->|\.)\w+|\[[^\]\n;]*\])*)\s*=\s*(?P=lhs)\s*"
                                  r"(?:<<|>>|[-+*/%&|^])\s*(?:\w|\.|->|\[[^\]\n]*\]|\([^()\n]*\))+\s*;"),
    "pasted-types": re.compile(r"^typedef\s+[a-z ]+\s+[usf](?:8|16|32|64)\s*;|^#define\s+(?:NULL|TRUE|FALSE)\b", re.M),
}
TYPE_NAME = re.compile(r"^\}\s*(\w+)\s*;|\btypedef\s+(?:struct|union)\s+\w+\s+(\w+)\s*;|\b(?:struct|union)\s+(\w+)\s*\{", re.M)
DECLARATION = re.compile(r"^[ \t]+" + TYPE + r"\s*(\w+)\s*(?:\[[^\]]*\])?\s*(?:=[^;,]*)?((?:,\s*\**\s*\w+\s*(?:\[[^\]]*\])?\s*(?:=[^;,]*)?)*);", re.M)
DECLARATOR = re.compile(r",\s*\**\s*(\w+)")
DEFINITION = re.compile(r"^\w[\w \*]*\b\w+\s*\(([^;{)]*)\)\s*\{", re.M)
PARAMETER = re.compile(r"(\w+)\s*(?:\[[^\]]*\])?\s*$")
PROTOTYPE = re.compile(r"^(?:extern\s+|static\s+|inline\s+)*([A-Za-z_][\w \t*]*?)\b(\w+)\s*"
                       r"\(([^;{}()]*(?:\([^()]*\)[^;{}()]*)*)\)\s*([;{])", re.M)
INLINE_ADDRESS = re.compile(r"\(\s*(?:const\s+|volatile\s+)*[\w ]+?\s*\*+\s*\)\s*(0x[0-9A-Fa-f]{7,8})\b"
                            r"|\b(0x7000[0-9A-Fa-f]{4})\b")
SCALAR_SPELLING = {"s32": "int", "signed int": "int", "unsigned int": "u32", "unsigned": "u32", "float": "f32",
                   "char": "s8", "signed char": "s8", "unsigned char": "u8", "short": "s16",
                   "unsigned short": "u16", "double": "f64"}
INCLUDE = re.compile(r'^#include\s+"([\w./]+)"', re.M)
MATCH_NOTE = re.compile(r"(?://|/\*)\s*match:")
COMMENTS_AND_STRINGS = re.compile(r"//[^\n]*|/\*.*?\*/|\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'", re.S)


def blank_out(match):
    """Replace a comment or string with spaces, keeping newlines so line numbers hold."""
    return re.sub(r"[^\n]", " ", match.group())


def read_code(path):
    """The file's text, and the same text with comments and strings blanked out."""
    text = open(path, encoding="utf-8", errors="replace").read()
    return text, COMMENTS_AND_STRINGS.sub(blank_out, text)


def is_short_name(name):
    return (len(name) <= 2 and name not in SHORT_NAMES_OK) or name in GENERIC_NAMES


def normalize_type(text):
    """Canonical spelling of a type, parameter names dropped: 'const Foo *bar[]' -> 'const Foo **'."""
    text = re.sub(r"\(\s*\*\s*\w*\s*\)", "(*)", text)
    text = re.sub(r"(\w+)\s*(?:\[[^\]]*\]\s*)+$", r"*\1", text.strip())
    tokens = re.findall(r"\w+|\*|\(|\)|\[|\]|\.\.\.", text)
    words = [token for token in tokens if re.match(r"\w", token)]
    if ("(" not in tokens and len(words) >= 2 and tokens[-1] == words[-1] and words[-1] not in TYPE_WORDS
            and words[-2] not in ("struct", "union", "enum")):
        tokens = tokens[:-1]
    result = ""
    for previous, token in zip([""] + tokens, tokens):
        if re.match(r"\w", previous) and (re.match(r"\w", token) or token in "*("):
            result += " "
        result += token
    return result


def split_parameters(text):
    parameters, depth, start = [], 0, 0
    for index, char in enumerate(text):
        depth += {"(": 1, ")": -1}.get(char, 0)
        if char == "," and depth == 0:
            parameters.append(text[start:index])
            start = index + 1
    parameters.append(text[start:])
    return [parameter for parameter in parameters if parameter.strip()]


def shape(type_name):
    """What declarations are compared on: any pointer is a pointer (pointer types wait on shared structs)."""
    if "(" in type_name:
        return "(*)()"
    if "*" in type_name:
        return "*"
    if re.fullmatch(r"(?:const )?(?:[A-Z]\w*|sceVu0\w+)", type_name):
        return "*"  # Vec, Mat, sceVu0FVECTOR... are array typedefs
    return {"s32": "int"}.get(type_name, type_name)


def prototypes(code):
    """Yield (line, name, signature, shape, is_definition) for each top-level function declaration."""
    for match in PROTOTYPE.finditer(code):
        return_type, name, parameters, end = match.groups()
        if name in ("if", "while", "for", "switch", "return", "sizeof"):
            continue
        if not parameters.strip():
            continue  # K&R "()" says nothing about the parameters
        types = [normalize_type(return_type)] + [normalize_type(p) for p in split_parameters(parameters)]
        signature = f"{types[0]} ({', '.join(types[1:])})"
        yield code.count("\n", 0, match.start()) + 1, name, signature, tuple(map(shape, types)), end == "{"


_signature_index = None


def signature_index():
    """name -> (canonical shape, its signature, where it comes from), over every game .c and .h."""
    global _signature_index
    if _signature_index is None:
        definitions, declarations, spelling = defaultdict(Counter), defaultdict(Counter), {}
        for path in game_files(".c", ".h"):
            for _, name, signature, form, is_definition in prototypes(read_code(path)[1]):
                (definitions if is_definition else declarations)[name][form] += 1
                spelling.setdefault((name, form), signature)
        _signature_index = {}
        for name, counts in declarations.items():
            if name in definitions:
                form = definitions[name].most_common(1)[0][0]
                _signature_index[name] = (form, spelling[name, form], "its definition")
            elif len(counts) > 1:
                form, count = sorted(counts.items(), key=lambda item: (-item[1], item[0]))[0]
                _signature_index[name] = (form, spelling[name, form], f"{count} of {sum(counts.values())} declarations")
    return _signature_index


def global_variables(code):
    """Yield (offset, storage, name, shape) for each file-scope variable declaration or definition."""
    code = re.sub(r"^[ \t]*#(?:[^\n]*\\\n)*[^\n]*", blank_out, code, flags=re.M)
    depth, start, text = 0, 0, []
    for index, char in enumerate(code):
        if char == "{":
            depth += 1
            if depth == 1:
                text.append("{")
        elif char == "}":
            depth -= 1
            if depth == 0:
                text.append("}")
                if re.search(r"\)\s*\{\}$", "".join(text)):  # a function body: nothing to declare
                    text, start = [], index + 1
        elif depth == 0 and char == ";":
            yield from _variables(start, "".join(text))
            text, start = [], index + 1
        elif depth == 0:
            text.append(char)


def _variables(start, statement):
    offset = start + len(statement) - len(statement.lstrip())
    statement = statement.strip()
    if not statement or re.match(r"typedef\b", statement):
        return
    declarators = [re.sub(r"=.*", "", part, flags=re.S).strip() for part in statement.split(",")]
    if "(" in declarators[0]:
        return  # a prototype (or a function pointer, which no rule here covers)
    if "{}" in declarators[0]:
        declarators[0] = declarators[0].rsplit("}", 1)[1]
    first = re.fullmatch(r"((?:extern|static)\s+)?(.*?)\s*(\**)\s*(\w+)\s*((?:\[[^\]]*\]\s*)*)", declarators[0], re.S)
    if not first or not first.group(2) or first.group(2) in ("struct", "union", "enum"):
        return  # a forward declaration names a type, not a variable
    storage = (first.group(1) or "").strip()
    base = re.sub(r"\s+", " ", re.sub(r"\b(?:const|volatile)\b", "", first.group(2))).strip()
    yield offset, storage, first.group(4), data_shape(base, first.group(3), first.group(5))
    for declarator in declarators[1:]:
        match = re.fullmatch(r"(\**)\s*(\w+)\s*((?:\[[^\]]*\]\s*)*)", declarator)
        if match:
            yield offset, storage, match.group(2), data_shape(base, match.group(1), match.group(3))


def data_shape(base, stars, dimensions):
    """What data declarations are compared on: a pointer, an aggregate (array, struct), or the scalar type."""
    if stars:
        return "pointer"
    if dimensions.strip() or re.match(r"(?:struct|union)\b|[A-Z]|sce", base):
        return "aggregate"
    return SCALAR_SPELLING.get(base, base)


_data_index = None


def data_index():
    """name -> (shape, where it comes from) for each global the game declares in more than one way."""
    global _data_index
    if _data_index is None:
        definitions, declarations = defaultdict(Counter), defaultdict(Counter)
        for path in game_files(".c", ".h"):
            for _, storage, name, form in global_variables(read_code(path)[1]):
                if storage == "extern":
                    declarations[name][form] += 1
                elif not storage:
                    definitions[name][form] += 1
        _data_index = {}
        for name, counts in declarations.items():
            if name in definitions:
                _data_index[name] = (definitions[name].most_common(1)[0][0], "its definition")
            elif len(counts) > 1:
                form, count = sorted(counts.items(), key=lambda item: (-item[1], item[0]))[0]
                _data_index[name] = (form, f"{count} of {sum(counts.values())} declarations")
    return _data_index


def common_headers():
    """Game headers that include common.h, so including them is as good."""
    headers = {"common.h"}
    for path in game_files(".h"):
        if '"common.h"' in open(path, encoding="utf-8").read():
            headers.add(os.path.relpath(path, GAME).replace(os.sep, "/"))
    return headers


def findings(path):
    """Yield (line, rule, text) for one file."""
    text, code = read_code(path)
    line_of = lambda offset: code.count("\n", 0, offset) + 1
    noted = set()
    for number, line in enumerate(text.split("\n"), 1):
        note = MATCH_NOTE.search(line)
        if note:
            noted.add(number + 1 if not line[:note.start()].strip() else number)
    for finding in _findings(path, text, code, line_of):
        if finding[0] not in noted:
            yield finding


def _findings(path, text, code, line_of):
    if os.path.basename(path) == "common.h":
        return  # where the typedefs live
    for rule, pattern in RULES.items():
        if rule == "local-struct" and path.endswith(".h"):
            continue
        for match in pattern.finditer(code):
            yield line_of(match.start()), rule, (match.groupdict().get("text") or match.group()).strip()
    for match in DECLARATION.finditer(code):
        names = [match.group(1)] + DECLARATOR.findall(match.group(2))
        for name in filter(is_short_name, names):
            yield line_of(match.start()), "short-name", name
    for match in DEFINITION.finditer(code):
        for parameter in match.group(1).split(","):
            name = PARAMETER.search(parameter.strip())
            if name and name.group(1) != "void" and is_short_name(name.group(1)):
                yield line_of(match.start()), "short-name", name.group(1)
    if path.endswith(".c"):
        for match in TYPE_NAME.finditer(code):
            name = next(group for group in match.groups() if group)
            if name in GENERIC_TYPES or len(name) <= 2:
                yield line_of(match.start()), "generic-type", name
        if not set(INCLUDE.findall(text)) & common_headers():
            yield 1, "missing-common", '#include "common.h"'
    index = signature_index()
    for line, name, signature, form, is_definition in prototypes(code):
        if not is_definition and name in index and index[name][0] != form:
            yield line, "proto-conflict", f"{name}: {signature}, but {index[name][2]} says {index[name][1]}"
    header = path.endswith(".h")
    for match in INLINE_ADDRESS.finditer(code):
        line_start = code.rfind("\n", 0, match.start()) + 1
        line_text = code[line_start:code.find("\n", match.start())]
        if header and line_text.lstrip().startswith("#define"):
            continue  # the one named definition
        if RULES["hw-define"].match(line_text):
            continue  # reported as hw-define
        yield line_of(match.start()), "inline-address", match.group(1) or match.group(2)
    index = data_index()
    for offset, storage, name, form in global_variables(code):
        if storage != "static" and name in index and index[name][0] != form:
            yield line_of(offset), "data-conflict", f"{name}: {form}, but {index[name][1]} says {index[name][0]}"


def git(*args):
    return subprocess.run(["git", *args], cwd=ROOT, capture_output=True, text=True, check=True).stdout


def changed_files(ref):
    """path -> lines to check: None for a new file (all of it), else the lines changed since REF."""
    base = git("merge-base", ref, "HEAD").strip()
    game = ["--", "src/game/*.c", "src/game/*.h"]
    files = {name: None for name in git("diff", "--name-only", "--diff-filter=A", "-M", base, *game).split()}
    files.update({name: None for name in git("ls-files", "--others", "--exclude-standard", "src/game").split()})
    # a moved file is checked on the lines that changed in the move, like a modified one
    for line in git("diff", "--name-status", "--diff-filter=MR", "-M", base, *game).splitlines():
        old_name, name = line.split("\t")[1], line.split("\t")[-1]
        files[name] = set()
        diff = git("diff", "-U0", "-M", base, "--", old_name, name)
        for start, count in re.findall(r"^@@ -\S+ \+(\d+)(?:,(\d+))? @@", diff, re.M):
            files[name].update(range(int(start), int(start) + int(count or 1)))
    return {os.path.join(ROOT, name): lines for name, lines in sorted(files.items()) if name.endswith((".c", ".h"))}


def report(paths):
    """Print the findings in each path (a dict limits each file to a set of lines)."""
    total = 0
    for path in paths:
        lines = paths.get(path) if isinstance(paths, dict) else None
        seen = set()
        for line, rule, text in sorted(findings(path)):
            key = (rule, text) if rule in NAME_RULES else (line, rule, text)
            if key in seen or (lines is not None and line not in lines):
                continue
            seen.add(key)
            print(f"{os.path.relpath(path, ROOT)}:{line}: {rule}: {text}")
            total += 1
    return total


def summary():
    paths = game_files(".c")
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
    for rule in sorted({*RULES, "short-name", "generic-type", "missing-common", "proto-conflict",
                        "inline-address", "data-conflict"}):
        count = files_per_rule[rule]
        print(f"  {rule:15} {count:5} files ({100 * count / len(paths):4.1f}%)")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("files", nargs="*", help="files to check in full")
    parser.add_argument("--changed", nargs="?", const="origin/main", metavar="REF",
                        help="check src/game files added, and lines changed, since REF (default origin/main)")
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
