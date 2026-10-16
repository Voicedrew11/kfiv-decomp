# Contributing

The goal is C source that the original compiler turns back into the exact
bytes of the retail game. Most of the work is taking one function at a time
from assembly to matching C.

## Rules

- **Never commit anything from the disc**: no ISO, ELF, `asm/`, `build/` or
  extracted files. `.gitignore` covers them; keep it that way.
- A pull request must build: `./build.sh` ends with
  `build/kfiv.rom: OK (matches retail)`.
- Commit only the sources you wrote (`.c` files under `src/game/`), plus
  `config/symbol_addrs.txt` if you give something a real name. Everything
  else the build regenerates; don't commit `docs/progress/` either, the
  maintainers refresh it.

## Setup

You need your own copy of *King's Field: The Ancient City* (USA,
SLUS-20318) as an ISO image, and 64-bit Linux (WSL2 should work, but is
untested) with 32-bit glibc, a host gcc that can build `-m32`, flex, bison,
curl, git and Python 3.
On Fedora: `sudo dnf install glibc-devel.i686 libstdc++.i686 flex bison`.

```sh
git clone https://github.com/Voicedrew11/kfiv-decomp.git   # a path without spaces
cd kfiv-decomp
mkdir -p baserom
ln -s "/path/to/King's Field The Ancient City.iso" baserom/kfiv_us.iso
./build.sh
```

The first run fetches and builds the toolchain into `tools/cc/` (a few
minutes), extracts the ELF, splits it with splat and builds. It ends with
`OK (matches retail)`.

## Matching a function

1. **Pick one.** Take an open issue (comment on it so nobody doubles up), or
   any function in `asm/`. Small ones are easiest; see the issues labelled
   `good first issue`.

2. **Get its assembly.**

   ```sh
   .venv/bin/python tools/match.py asm func_00102228
   ```

3. **Write a first draft.** [decomp.me](https://decomp.me) is the easiest
   way: new scratch, platform PS2, compiler **EE GCC 2.9 build 991111-01**
   (`ee-gcc2.9-991111-01`), flags `-O2 -G8`. Paste the assembly. Its m2c
   decompiler gives a draft to start from.

4. **Put it in the tree** as `src/game/func_XXXXXXXX.c`, named after the
   function. Declare what it uses at the top of the file, using splat's names:

   ```c
   extern int D_00446710;

   void func_00102228(void) {
       D_00446710 = 0;
   }
   ```

5. **Check it.**

   ```sh
   .venv/bin/python tools/match.py diff src/game/func_00102228.c
   ```

   compiles the file with the game's flags and compares it with the retail
   bytes, one instruction per line (`*` marks a difference). Repeat until it
   says `OK`.

6. **Build.** `./build.sh` notices the new file, re-splits and links. It must
   end with `OK (matches retail)`.

7. **Open a pull request** with the new `.c` file(s). If you understood what
   the function does, say so in the description; that is how names get
   chosen.

## Ghidra (optional)

For big functions, Ghidra's decompiler and cross-references help.
`tools/ghidra_mcp.sh` serves a headless Ghidra project over MCP
([pyghidra-mcp](https://github.com/clearbluejar/pyghidra-mcp)), so an AI
assistant such as Claude Code can decompile, disassemble and follow xrefs.
You need Ghidra 12.x, JDK 21, and the
[ghidra-emotionengine-reloaded](https://github.com/chaoticgd/ghidra-emotionengine-reloaded)
release for your Ghidra version unzipped into `Ghidra/Extensions/`.

```sh
uv tool install --python 3.13 pyghidra-mcp
export GHIDRA_INSTALL_DIR=/path/to/ghidra_12.x_PUBLIC
tools/ghidra_mcp.sh import          # imports build/kfiv.elf, ~1 minute
claude mcp add ghidra -- "$PWD/tools/ghidra_mcp.sh"
```

The import uses `build/kfiv.elf`, so Ghidra has the same function names and
sizes as splat. Run the import again after `./build.sh` to pick up new names
(it discards anything renamed inside Ghidra). The project goes in
`~/ghidra-projects/kfiv/` unless `KFIV_GHIDRA_PROJECT` names another `.gpr`.

## Notes on this compiler

The game is built with ee-gcc 2.9-ee-991111-01, `-O2 -G 8`.

- A value in `$2` before `jr $31` is not always a return value. If the
  caller never reads it, the function is `void` and `$2` was scratch.
- At `-O2` the scheduler reorders independent stores. Two stores of one value
  in retail order `a, b` often come from `a = b = x;`.
- Variables of 8 bytes or less are reached through `$gp` (`-G 8`). Declare an
  array with its real size (`extern int D_004467D8[2];`): an incomplete
  `extern int x[];` makes gcc use `lui`/`addiu` instead.
- A `lwc1` through `$gp` from a `D_...` in `.lit4` is a float literal: write
  the literal, not an `extern float`. gcc emits it as `li.s`, which the
  delay-slot filler leaves alone. The assembler builds a value whose low 16
  bits are zero (`1.0f`) with `lui`; every other `li.s` gets its own `.lit4`
  entry, in code order, with no sharing even of equal values. The
  file then owns that stretch of the pool, and `tools/carve.py` places it.
- A loop counting down to zero with `bnez` usually comes from an ordinary
  `for (i = 0; i < n; i++)` loop with `n` in a local variable.

## Several functions in one file

The original game was many C files with several functions each. Functions
that share a string, a `switch` jump table or a `static` variable must end up
in the same file. `tools/tu_scan.py` lists those groups in
`build/tu_groups.txt`, and `tools/tu_scan.py --check START END` says whether
a range of functions may be one file. While a function's data stays in the
assembly (the `extern D_...` style above), one function per file is fine.
See step 3 of [`docs/ROADMAP.md`](docs/ROADMAP.md).

## Readable C

Matching comes first, but keep the source readable. Write it the way a
programmer would have, not the way m2c printed it:

- `#include "common.h"` (or a `kf_*.h` header that includes it) for `u8`,
  `s32`, `NULL` and friends. Delete the typedef block decomp.me pastes in.
- Use the shared structs in the headers under `src/game/` (e.g. `Entity` in `entity/entity.h`)
  instead of a local struct or raw casts like `*(int *)(p + 0x10)`. If you
  find a new field, add it to the header (`unkXX` until you know what it is).
- Struct-shaped data is a struct, not an array: not `extern u8 D_00413F80[]`
  read as `D_00413F80[0x261]`, not an `int *` parameter read as `arg0[7]`.
- Unknown fields are `unk1B4` and padding `padD0[0xE4]`, never
  `f1B4`, `s44` or `p0C`. When a local struct can't be avoided yet, name it
  for what it is (`TargetMenuState`), not `Obj`, `Ctl`, `Actor`, `Sub` or `A`.
- Use long, descriptive names for variables and fields
  (`distance_to_player`, not `d` or `f64`). Name a function
  (`Entity_FreeAllInGroup`) once its purpose is clear; see Names below.
- Naming convention: functions `Module_Verb` (`Vector_Subtract`),
  variables and fields `snake_case`, types `PascalCase`, constants and
  macros `UPPER_CASE`.
- Write `NULL` for pointers, `TRUE`/`FALSE` for flags, and name magic
  numbers (`ENTITY_DESPAWN_MARGIN`, `GS_REG_ZBUF_1` from `gfx/gs.h`), flag
  bits included: `ENTITY_FLAG_40000` beats a bare `0x40000`. Macros like
  `ABS()` expand to the same code, so they still match.
- EE registers come from `eeregs.h` (`*D1_CHCR`, `*GIF_STAT`), not a local
  `#define HW_10009000`.
- No raw addresses in code. Game data is always a symbol (`D_00446710`), and
  fixed addresses such as the scratchpad (`0x70000000`) get one named
  `#define` in a shared header, not `*(int *)0x70000010` in each file.
- Idiomatic C: `x += y` not `x = x + y`, `if (a && b)` not nested ifs. gcc
  2.9 almost always emits the same code.
- A function is declared the same way everywhere, matching its definition:
  module prototypes go in the module header (`entity/entity.h`), and a function
  that is still asm keeps the signature other files already use. If a match
  needs a different one, the others are wrong; fix them too.
- Code that is odd only to match (`new_var`, a pointless cast, a union for a
  byte load) gets a meaningful name and a one-line `// match:` comment, so
  nobody "cleans it up" later.
- Keep comments brief: one line on what a function or field is. No essays
  on trivial code.

`Entity_DespawnOutOfRange` in
[`entity/entity_pool.c`](src/game/entity/entity_pool.c) is the style to aim for.

`tools/lint_readability.py --changed` must pass (CI runs it): files your
PR adds must be clean, and so must the lines it changes in other files. A
`// match:` comment
on the line, or on its own line just above, silences a finding the match
forces. The lint is a regex scan, so if it is plainly wrong, say so in the
PR. Rebuild after fixing anything. Run it with no arguments for a summary of
the whole tree.

`tools/lint_symbols.py --changed` must pass too (CI runs it). It checks the
whole tree: `config/symbol_addrs.txt` in address order with no duplicates,
no `func_`/`D_` name left where the
address has a real name, every `Module_Verb` function in `symbol_addrs.txt`,
functions in address order within a file, and no generated or disc-derived
file in the change.

## Names

Keep `func_XXXXXXXX` and `D_XXXXXXXX` until you know what something is. A
real name goes into `config/symbol_addrs.txt` (in the game section, in
address order), e.g. `Entity_FreeAllInGroup = 0x001231C0; // type:func`, and the C
uses the new name. `KF_File_Open` keeps its prefix: it is the one name the
game itself gives away (in a debug string), so it is FromSoftware's own.