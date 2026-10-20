# kfiv-decomp

A matching decompilation of **King's Field: The Ancient City** (*King's Field IV*;
FromSoftware, 2001) for the PlayStation 2. The target is the USA retail boot
ELF, `SLUS_203.18` (SLUS-20318). The goal is C source that the original
toolchain compiles back into the identical executable.

[![code](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/Voicedrew11/kfiv-decomp/main/docs/progress/badge-code.json)](docs/progress/) [![functions](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/Voicedrew11/kfiv-decomp/main/docs/progress/badge-functions.json)](docs/progress/) [![game named](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/Voicedrew11/kfiv-decomp/main/docs/progress/badge-named.json)](docs/progress/) [![structs recovered](https://img.shields.io/endpoint?url=https://raw.githubusercontent.com/Voicedrew11/kfiv-decomp/main/docs/progress/badge-structs.json)](docs/progress/)

**Status:** the build reproduces the retail ROM image (all bytes the console
loads) **byte for byte**. The SDK, libc, libm and libgcc code is in C (181
units in `src/sce/`, about 12 % of `.text`); the game itself is still
splat-disassembled assembly. See [`docs/ROADMAP.md`](docs/ROADMAP.md).

> No disc data is in this repository. You supply your own disc image; the
> build extracts the ELF from it and checks its SHA-1.

## Quickstart

```sh
mkdir -p baserom
ln -s "/path/to/King's Field The Ancient City.iso" baserom/kfiv_us.iso
./build.sh
```

`build.sh` runs whatever is missing:

1. `tools/setup.sh` creates `.venv`, fetches the period compilers and a
   pinned objdiff-cli into `tools/cc/`, and builds GNU ld 2.10 and dvp-as
   from source.
2. `tools/extract_elf.py` writes `baserom/us/SLUS_203.18` and
   `baserom/us/kfiv.rom` (the ELF's load segment) and checks both SHA-1s.
3. `tools/split.py` runs splat and writes `asm/`.
4. `tools/gen_ninja.py` and `ninja` assemble, compile and link with ld 2.10,
   then check the ROM SHA-1.

Host requirements: 64-bit Linux with 32-bit glibc (the compilers are i386
binaries), a host gcc with `-m32` support, flex, bison, curl, git and Python 3.
Fedora: `sudo dnf install glibc-devel.i686 libstdc++.i686 flex bison`.

| file | SHA-1 |
| --- | --- |
| `SLUS_203.18` | `8031536201dec92889d3013ca80f09374571d626` |
| `kfiv.rom` (PT_LOAD bytes, i.e. `objcopy -O binary`) | `87615665d1a4a88e57942126a03ab7ad03cd42c6` |

## Toolchain

KFIV is plain C. It was built with the same toolchain and the same Sony SDK
release as ICO, whose decompilation
([nathanialf/ico](https://github.com/nathanialf/ico)) is complete. More than
680 SDK, libc and libgcc functions compiled from ICO's reconstructed sources
are **byte-identical** to code in KFIV.

| tool | role |
| --- | --- |
| ee-gcc 2.9-ee-991111-01 (`-O2`) | compiles everything |
| ee-as 2.9-991111 (bundled) | assembles libc, libm and libgcc, and splat's asm |
| ee-as 2.9-991111, delay-slot filling (one-byte patch built by `tools/compile_c.sh`) | assembles the game's C |
| SCE ee-as 2.10-ee-001003-1 (from ee-gcc 2.96) | assembles Sony's SDK libraries |
| GNU ld 2.10 + R5900/DVP patches | links |
| dvp-as | the VU1 microprograms (not wired in yet) |

SDK library versions in the binary: libkernl, libcdvd and libmc 2.2.4;
libgraph, libdma, libpad, libipu, libmpeg and libsdr 2.2.0.

## Layout

```
config/    splat yaml template, symbol_addrs.txt, linker-script template,
           sha1sums.txt
include/   macro.inc (splat asm macros for ee-as 2.9)
src/       C sources as they are decompiled (src/game/, and src/sce/ for
           the libraries)
src/game/  the game, a folder per subsystem; headers are included by
           their path under src/game ("entity/entity.h"):
             core/        start-up, main loop, game state, memory, files,
                          timers, ini settings
             debug/       debug tools, text log, host0 dumps
             math/        angles, vectors, VU0 helpers, shared matrices
             gfx/         GS packets and drawing, textures, screen effects,
                          movies
             model/       models, motions, model matrices, linked points
             map/         map loading, changing, set-up, drawing state
             map_object/  map objects (doors, chests, switches ...) and
                          their tables
             collision/   collision tests and the map's collision meshes
             script/      the map script interpreter and its commands
             entity/      entity pools, the shared AI, entity events, NPCs
             enemy/       one file per monster
             effect/      effects, effect kinds, shots, item effects
             player/      the player: update, status, items, attacks
             item/        item tables, items lying on maps, gold
             input/       pads and buttons
             sound/       the sound driver and the game's sound layer
             ui/          menus, HUD, messages, title, trade screens,
                          map viewer
             save/        memory card and save files
           main.c, common.h and the not-yet-named func_*.c files sit
           at the top
tools/     setup, extraction, carve, split, build generation, matching and
           check scripts
docs/      roadmap and notes
asm/  baserom/  build/  tools/cc/  .venv/    generated or local, gitignored
```

## Progress

```sh
.venv/bin/ninja progress
```

writes `build/report.json` with objdiff-cli: matched code and data, by bytes
and by function, overall and for the `game` and `sce` (libraries)
categories. `tools/gen_ninja.py` writes the `objdiff.json` it reads, one unit
per entry in the link order. Each C unit is compared against a target object
assembled from splat's per-function asm of the retail code, so the objdiff
GUI also works on this checkout. A checkout set up before objdiff-cli was
added needs `tools/setup.sh` run once more.

`ninja progress` also prints the summary and writes `build/progress.html`, a
local per-function page (not committed). Before that it runs
`tools/readability_scan.py`, which measures how readable the game C is past
its function names: structs with no `unkXX` field left, named fields (padding,
`padXX` bytes no game code touches, is counted apart), globals and flag bits, and counts of casts, `// match:` comments, gotos and
long comments still to clean up. `tools/progress.py --write` refreshes
`docs/progress/` (a summary without per-function data, shields badges and a
static page) and the table below; commit those with each batch of work.
`docs/progress/index.html` can be served with GitHub Pages (Settings → Pages →
`main`, `/docs`).

<!-- progress:start -->
| | code | linked | functions | named | units linked |
| --- | ---: | ---: | ---: | ---: | ---: |
| overall | 99.58% | 99.58% | 3837 / 3839 | 3838 / 3839 | 411 / 455 |
| game | 99.53% | 99.53% | 2990 / 2991 | 2991 / 2991 | 207 / 249 |
| sce | 100.00% | 100.00% | 847 / 848 | 847 / 848 | 204 / 206 |

| game readability | | |
| --- | ---: | ---: |
| structs recovered | 587 | total_structs |
| struct fields named | 3112 | total_fields |
| padding fields | 624 | unused by game code |
| field uses named | 58513 | field_uses |
| globals named | 1385 | total_globals |
| bit operations named | 3081 | bit_ops |
| function-pointer casts | 164 | left |
| `// match:` comments | 941 | left |
| gotos | 14 | left |
| multi-line comments | 1798 | left |
<!-- progress:end -->

## Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md): pick a function, write it in C,
check it with `tools/match.py`, run `./build.sh`, open a pull request.

## How units get into the build

`tools/carve.py` compiles every unit under `src/sce/` and `src/game/`, places
each in the retail ELF with `tools/lib_layout.py` (every section's bytes must
match) and writes `build/kfiv.us.yaml` (the template `config/kfiv.us.yaml`
plus a subsegment per unit), `build/link_order.us.txt` and
`build/symbol_addrs.carve.txt`. `./build.sh` runs it whenever `src/` or
`config/` changed, and splits again when the units changed. A unit owns its
stretch of `.text`, `.data`, `.rodata`, the float literal pool `.lit4`,
`.sdata` and `.bss`; the stretches between units stay assembly. Adding a unit is adding
its source file. `tools/tu_scan.py` reports which game functions must
share a unit (see [`docs/ROADMAP.md`](docs/ROADMAP.md), step 3).

## Credits and licence

This repository is MIT-licensed ([`LICENSE`](LICENSE)). It holds no game
data; the game itself belongs to its rights holders.

The binutils patches, the obstack shim (`tools/period_obstack.c`) and the
toolchain recipe come from the ico decompilation (MIT, © the ico
decompilation contributors; notice in [`tools/LICENSE.ico`](tools/LICENSE.ico)). This project has no affiliation with
FromSoftware or Sony Interactive Entertainment.
