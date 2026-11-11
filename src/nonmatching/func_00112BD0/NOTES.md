# Map_UpdateDrawState (0x00112BD0): state of the match

Not matched. `src/nonmatching/func_00112BD0.c` compiles against the current headers and gives 31 of 1393
instructions different, all in one basic block: the fog blend at 0x1131DC-0x1132E0. The block has retail's
instructions in another order. The ROM is still built from the function's assembly. The header comment of the
C file is the full record, pass by pass; this page is the short version for whoever tries next.

## What is established

**The block is compiler output from plain C.** Every run of instructions that stays together is one of this
compiler's templates (`mtc1; cvt.s.w`, `cvt.w.s; mfc1`, `lui $1; mtc1` for 30.0f, `nop; nop; div.s`), the registers
are the allocator's, and five statements are interleaved instruction by instruction. Inline asm and other
compiler builds were tested and excluded (see below).

**The statement shape is the plain one.** With the floats first and no register pins:

```c
fade_out = map_draw_state.unk9A;
fade_in = 30 - map_draw_state.unk9A;
fade_out /= 30.0f;
fade_in /= 30.0f;
dst.fog_base_value = from.fog_base_value * fade_out + to.fog_base_value * fade_in;
far_to = to.fog_inverse_depth_scale;
far_to *= fade_in;
dst.fog_inverse_depth_scale = from.fog_inverse_depth_scale * fade_out + far_to;
blended = (int)(FOG_COLOR(&from)->red * fade_out + FOG_COLOR(&to)->red * fade_in);
FOG_COLOR(&dst)->red = blended;
/* green and blue the same way, through the same int */
```

(`dst`, `from`, `to` = `map_draw_state.unkA0`, `.unk140`, `.unk160`.) The stock compiler gives 39 on this form.
The 31 of the form kept in the C file comes from a count-minimising search and says nothing about the original.

**What separates that source from retail is five scheduling dependences.** They were found with a copy of the
compiler rebuilt from source (github.com/SSXModding/ps2-ee-toolchain, `ee/gcc`, with the four output templates of
the 991111-01 binary patched in) and a hook in `haifa-sched.c` that dumps and injects dependence links. With
these injected, the plain source above compiles to all 1393 instructions of retail:

| pass | dependence |
| --- | --- |
| first scheduling pass only | the from-blue conversion is issued before the green multiplies (any extra dependent of it does this) |
| second pass | each of the four float loads (`lwc1` 0x148, 0x168, 0x14C, 0x16C off `$9`) after the to-blue conversion |
| second pass | `addiu $2,$9,0xB0` after both near-distance multiplies |
| second pass | the far "to" load after the far "from" load |

All are cost-1 (anti or output) links. The first one is a register matter: every FP value of the block is a
local-alloc quantity, allocated by `floor_log2(refs) * refs / life` in first-pass order, and the from-blue value
is allocated too early unless its life is four insns longer. One `register ... asm("$f5")` pin on that value
alone gives every register and 26 with the stock compiler.

**No C construct found makes the stock compiler create them.** Two facts narrow where they can come from:

- The second pass only derives dependences that point backward in its input, and an inherited link was obeyed
  by the first pass. So in retail's first-pass output the float loads already stood after the to-blue conversion
  (and before the red float-to-int conversion, whose scratch is `$f11`), and the `addiu` after the near multiplies.
- With the same links present in the first pass, the final instruction order is retail's everywhere and only
  three FP registers form a cycle (to-blue, from-blue, near-from): 13 differ, 10 for the best first-pass order found.

## Ruled out by test

- Compiler flags: about 150 sets in total.
- Other compiler builds: 2.96; 2.9-ee-990721 (does not match the rest of the module); 2.9-ee-991111 and the
  "991111a" build (both lack the `nop; nop` before `div.s`).
- Symbol names, declaration order and layout, include order, `-g`, the neighbouring translation units, host
  allocation behaviour: 33,000 compiles, one result per source form.
- Source shapes: every order of the statements, operand orders, shared float and int temporaries (about 20,000
  forms over all passes), pointer and struct views of the colours, direct `u8` members (no materialised colour
  addresses: retail's access is pointer-like), `volatile`/`const`, local copies, `do { } while (0)` wrappers
  (they align the next label), per-field statement macros, inline helpers of several signatures, a loop over the
  channels, statement and comma expressions, register-held structs and unions, compound literals (they emit a
  `clobber` that is a store-like barrier in the first pass only, but land in the wrong place).
- Values defined in an earlier block or left without a register: `update_equiv_regs` moves only single-use
  constants, cross-block loads never return to the block, and reload gives a three-register multiply, an `la`
  pair for an address and `$f7` as the red scratch.
- One pointer variable for the "to" colour and the destination colour: stores through it force a re-load of
  `current_map_slot` that retail does not have.
- Insns that disappear after scheduling: conditionals that are folded late (`if (c) x = v;` with `x` overwritten
  splits the block in the first pass only, without a trace, but adds no second-pass link), identical-arm
  `if/else`, no-op copies (`reload_cse_regs` deletes them before the second pass), unused, pinned and
  `__label__` labels.
- Scheduler internals: loop, EH and setjmp notes; the pending-list flush; `SCHED_GROUP_P` (never set in this
  function); stale flow or combine links; `REG_DEAD` notes treated as uses (not in this version); a real split
  into three blocks; stale block tables.
- Inline asm: the conversions as asm with register operands, and empty asm statements carrying operands, at
  every statement boundary (152,000 compiles). Best 30; none supplies any of the five dependences.
- Invisible insns in the second pass: `set`, `clobber` or `use` of every hard register, memory use, clobber and
  store, FPU and move insns, at every position; every choice of the hidden GPR scratch of the three
  `fix_truncsfsi2` insns; pairs of the best (370,000 compiles). Best single 13, best pair 5.

## Not yet tried

- Solve the form with the links already in the first pass, using local-alloc's real quantity table as the
  objective instead of the instruction count. Retail's registers need, in first-pass insn distances:
  `(blue add - to-blue conversion) <= (blue fix_trunc - from-blue conversion) / 1.5`; the near add at most one
  insn before the far add; `(near add - near "to" load) <= (far add - far "to" load)`. If that reaches 0, the
  whole difference is a first-pass matter and the question becomes which pseudo-level relation ties the float
  loads to the to-blue conversion and the `addiu` to the near multiplies.
- The FPU unit of the r5900 description issues one FPU insn per cycle in priority order, and the block has one
  in every cycle from 9 to 30. So the anchors need not be the insns named above: a conflict of the float loads
  with any FPU insn of priority 15-16, or of the `addiu` with one of priority 9-10, would do. Nobody has
  enumerated the source forms that could give such a conflict.
- Temporaries shared with the other blends of the function: a pseudo used in more than one block is allocated
  by global-alloc and leaves local-alloc's competition without changing the first pass's order. Only a few
  shared-variable forms were tried.

## Tools

The scripts in this folder predate the work above (permuter setups and early experiments). The compiler hook,
the harnesses and the per-pass notes are not in the repository; ask Acranon for them.
