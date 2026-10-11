# Roadmap

## Where things stand (2026-10-11)

- [x] Toolchain identified: ee-gcc 2.9-ee-991111-01 and SDK 2.2.x, the same as
      ICO (ICO's compiled library objects byte-match KFIV)
- [x] Disc extraction with SHA-1 checks
- [x] splat split (one asm unit for `.text`, one per data section)
- [x] All-asm build with the period assembler and ld 2.10 → **ROM SHA-1 matches**
- [x] `tools/compile_c.sh`, tested by compiling ICO's `libdma.c` to KFIV's
      libdma bytes (0x0022A1D8, 0xA74 bytes)
- [x] Library translation units carved: 181 SDK, libc, libm and libgcc units
      in `src/sce/` (C, plus newlib's and the SDK's hand-written `.s`), 0x289DC
      bytes of `.text` (about 12 %), with their data, still **SHA-1 matching**
- [x] objdiff progress reports: `ninja progress` → `build/report.json`
      (162 of 167 library C units at 100 % matched code)
- [x] Progress page and README badges from the report (docs/progress/)
- [x] First game functions matched in C; game flags settled: `-O2 -G 8`, no
      `-g` (see step 4)
- [x] Game translation-unit constraints: `tools/tu_scan.py` (the binary fixes
      which functions share an object, not where objects end)
- [ ] Group game functions into units as the code is understood
- [ ] Whole-ELF match (section headers, `.DVP` overlays, no symtab)

## Memory map (retail ELF)

| section | VMA | size | notes |
| --- | --- | --- | --- |
| .text | 0x00100000 | 0x149668 | crt0 + game ≈ 0x100000–0x216778; SDK/libc after |
| .vutext | 0x00249668 | 0 | |
| .data | 0x00249680 | 0x1F1B48 | 2 MB, much of it game tables |
| .vudata | 0x0043B1D0 | 0x2790 | VU DMA/microcode data |
| .rodata | 0x0043D980 | 0x71D8 | |
| .lit4 | 0x00444B80 | 0x1B74 | float literals |
| .sdata | 0x00446700 | 0x8E4 | |
| .sbss | 0x00447000 | 0x404 | |
| .bss | 0x00447480 | 0x1A107F0 | 26 MB |
| .DVP.overlay × 8 | 0 | | VU1 microprograms, not in the ROM view |

`_gp` = 0x0044CB70. splat finds 3,843 functions; 684 of them already have
names from the library match.

## Next steps, in order

### 1. Carve out the library translation units (done)

`tools/carve.py` compiles every `.c`/`.s` under `src/sce/` and `src/game/`,
places each unit and each of its sections in the retail ELF
(`tools/lib_layout.py`: masked byte search for `.text`, relocations for data,
link order for what nothing references), and writes the splat subsegments
(`build/kfiv.us.yaml`, from the template `config/kfiv.us.yaml`),
`build/link_order.us.txt` and `build/symbol_addrs.carve.txt`. These are build
products: `./build.sh` runs carve whenever `src/` or `config/` changed and
splits again when its output changed, so adding a unit is adding its file,
and pull requests never touch generated files. Code that occurs more than
once is placed by the names in `config/symbol_addrs.txt` (a few library
members are pinned there) or, for a game function still called
`func_XXXXXXXX`, by the address in its name.

Most units are ICO's sources. New for KFIV: libsdr (one unit for the whole
archive), libdev's VU0/VIF0/VIF1 functions that compile from C (one unit per
function, names inferred), the double-precision fdlibm members, mktime,
strcasecmp and strchr, and a few libgraph, libcdvd, libkernl and libc
members ICO did not link.

Still asm in the library range: the libdev functions that are mostly inline
asm (`sceDevVu0GetVi/GetVf/PutMem/GetMem/ChkIdle/GetCnd/GetCtx/PutCtx` and
the VIF0/VIF1 reset, pause, continue, status, register and FIFO functions).
Their hand-copied `.s` would only rename splat's output, so they wait for C.
Also asm: 0x24 bytes of unreferenced `.bss` at 0x01E548DC and the nop fill at
the end of `.text`.

### 2. Progress reporting with objdiff (done)

`tools/gen_ninja.py` writes `objdiff.json`, one unit per entry in the link
order, in the categories `game` and `sce`:

- C units: the base object is `build/src/<path>.c.o`; the target object,
  `build/target/src/<path>.o`, is assembled from splat's per-function asm
  (`asm/matchings/`) in address order plus the unit's data files, with
  splat's fill before the next unit trimmed off.
- Hand-written `.s` units are both target and base (matching by
  construction, marked complete). splat's own `asm/` units are target only.

`tools/cc/objdiff-cli` (pinned in `tools/setup.sh`) writes `build/report.json`
from `ninja progress`. For splat to cut the target the way the C objects are
cut, `tools/carve.py` names every static function and object of the library
units (with sizes), and lists with `ignore:True` the constants the retail code
builds with lui/lo16 pairs that splat would take for addresses.

Five library functions still show a relocation-only difference (99.7 % or
more fuzzy): `_sceFs_Rcv_Intr` (a `%hi` with no `%lo` partner, which splat
cannot symbolize), `_sceCd_cd_read_intr`, `_sendDataToIPU` and
`mceIntrReadFixAlign` (an address built through `or`, which splat does not
follow) and `_copyRefImage` (`%hi/%lo(_maxval)` of a table inside the
function). Matched data is low: splat writes unlabelled string literals as
word runs, so a target `.rodata` cannot end on the C object's byte.

`tools/progress.py` summarises the report in the terminal and, with `--write`,
refreshes `docs/progress/` (a summary without per-function data, shields
badges and a static page) and the README table. The disc is needed to build,
so CI cannot produce the report: it is generated locally and the summary is
committed with each batch of work. `ninja progress` also writes a local per-
function page to `build/progress.html`.

### 3. Find the game's translation-unit boundaries (constraints done)

There is no link map (ICO's PAL disc shipped one; KFIV's does not), and the
binary does not record where one game object ends. Checked and ruled out:

- `.text` padding: ee-as pads every function to 8 bytes, the same alignment
  as an object's `.text`, so an object end leaves no extra `nop`.
- Data order: a file's statics are emitted at its end in declaration order,
  which follows use order, so data addresses rise smoothly across a boundary.
  Cuts that look clean in `.bss` order are contradicted by independent
  evidence more often than random cuts are.
- `.lit4`: one 4-byte-aligned entry per load, in code order, never shared.
- gp-relative against absolute access: no small-data item is reached both
  ways, so no file declares a variable differently from another.
- Names: under ten strings name a function or file, and the Holyland CSV
  has only Ghidra's generated names.
- Calls: a cut no nearby call crosses is only slightly more likely to be
  real (contradicted 24 % of the time against 33 % for any cut).

What the binary does fix is which functions must share an object.
`tools/tu_scan.py` (writes `build/tu_groups.txt`) finds them from the data an
object keeps to itself:

- String literals (once per object) and jump tables in `.rodata`/`.sdata`.
- `.bss` and `.sbss`: the game has no COMMON block and its `.bss` precedes
  the library's, so these hold only file-static variables. Every item is used
  within at most 100 functions; `.data` items span up to 2,757.
- An item placed after another but first used before the other's last use
  ties every function in between to one object.

Result: 39 runs of two or more functions (1,036 of 2,990, the largest 104)
that each lie inside one object, and 2 strings with two copies, which put a
boundary between their users. Any partition that keeps the runs whole and
makes those 2 cuts links to the same bytes; the real files are some
coarsening of it, to be chosen as the code is understood.

`tools/tu_scan.py --check START END` reports whether a unit may cover
`[START, END)`. A C unit that owns its strings, jump tables or statics must
cover whole runs. The current one-function units in `src/game/` mostly sit
inside runs, which is fine while their data stays in the asm data units.

### 4. Match the game functions (started)

- Flags: `-O2 -G 8`, the same optimisation as the SDK. Leaf functions match at
  `-O1` and `-O2` alike and settle nothing; loops with calls
  (`func_00102558`, `func_00123158`, `func_0019CCB0`) match only at `-O2`.
  `-g` changed no code in any game function or in 167 of 168 library units
  compiled both ways. In gcc 2.9 its line labels only stop ee-as from moving an
  inline-asm instruction into a branch delay slot (libvu0's
  `sceVu0RotTransPers`), and no game branch shows that pattern. It is left
  off; `GAME_CFLAGS` overrides it.
- A store whose register happens to be `$2` is not a return value:
  `func_00120D78` is `void`. Small functions with two stores of one value come
  out in the scheduler's order, so their C is a chained assignment.
- Each matched function is its own unit in `src/game/` until step 3 gives
  the real file boundaries. A unit may keep its own string literals:
  `tools/carve.py` splits `.rodata` and `.sdata` around it (`KF_File_Open`'s
  `"\\DATA\\"` and `";1"` are 8 bytes or less and go to `.sdata`). `.lit4` and
  `.sbss` are still single asm units.
- Workflow per function: m2c for a first draft → decomp.me (preset "PS2
  ee-gcc 2.9-991111-01") or objdiff → drop into `src/`.
- Start with leaf utility code (math, string, file wrappers such as
  `KF_File_Open`) and work outward.

### 5. Whole-ELF match (later)

The ROM view matches already. A full `SLUS_203.18` SHA-1 match also needs the
empty `.vutext` header, `.reginfo` placement, the eight `.DVP.overlay`
sections (rebuilt from VU1 microcode with dvp-as) and no `.symtab`. ICO's
README describes how they got there.

## Build notes and gotchas

- The checkout path must not contain spaces: binutils' autoconf trees
  (`tools/setup.sh`) and `LD_PRELOAD` (`tools/period_env.sh`) both break on
  them, and both scripts stop with a clear error. Everything the build needs
  lives inside the checkout: the toolchain and its sources under `tools/cc/`
  and the obstack shim under `build/` (all gitignored and regenerated).
- ee-as 2.9 syntax limits are handled in `tools/split.py`: numeric registers,
  no `%gp_rel` (rewritten to `.extern sym, 4` plus the macro form), bare
  `ACC`/`Q` for VU0, floats emitted as `.word`, and EUC-JP strings kept as
  bytes.
- ld 2.10 has no `INCLUDE` inside an output section, so `gen_ninja.py`
  expands `config/kfiv.us.ld.in` into `build/kfiv.ld`.
- `.splache` is ignored and `split.py` always does a full split.
