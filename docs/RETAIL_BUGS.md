# Bugs in the retail game

Bugs found in the original code while matching it. The C in `src/` reproduces
each one, because the build has to match retail byte for byte. This list is
for the port: every entry is a place where a port may want to behave
differently, or must take care to behave the same.

Add an entry when a matched function does something the programmer clearly
did not intend. Give the function, what it does, and what the effect is if
you know it. Say so when the effect is a guess. In the source, mark the line
with a `// match:` comment so nobody "fixes" it.

## Entity target left pointing at a dead stack local

`Entity.target` (0x5C) is the point an entity walks toward. Three functions
build that point in a local array and store the array's address in
`entity->target` before returning, so the pointer outlives the array:

| Function | Local | What it is doing |
| --- | --- | --- |
| `func_00165280` | `point` | jumping kind, picks a wander point near spawn with `func_0018B778` |
| `func_00157E70` | `retreat_point`, `destination` | walks to a point from `func_0018BA10` when the player is far above or below |
| `func_0013F1B8` | local point | chase that stops at a post |

Effect: whatever reads `target` later in the frame, or in a later frame,
reads stack memory that other calls may have overwritten. It probably works
in retail because the movement code runs soon after, while the old values are
still there. Not checked: which readers run before `target` is next assigned.
A port that changes stack layout or call order can break this silently; give
these entities real storage for the point.

## `func_001686B0` reads `angle` before setting it

The AI decision function of the kind initialised by `func_00168160` tests
`ABS(angle) < 1.0471976f` (60 degrees) in an early branch, but `angle` is only
assigned further down. The test uses whatever was in the register.

Effect: the branch (a `rand() % 100 < params->unk434` roll) is taken or not
depending on leftover register contents. Intended value unknown; most likely
the angle to the player that the later code computes.

## `func_00185480` copies the wrong component of the push vector

The enemy collision function sums the push-out normals into `push[4]`,
normalises it, and stores it in `Entity.unkB0[3]`:

```c
entity->unkB0[0] = push[0];
entity->unkB0[2] = entity->unkB0[1] = push[2];
```

`unkB0[1]` gets `push[2]` (z), not `push[1]` (y).

Effect: the stored push direction has its y replaced by its z. `unkB0` is read
by the return-to-spawn code (`func_0015B428`, `func_00182DE0`), which turns
away from pushes; those use it for a horizontal cross product, so the wrong y
may never be seen.

## `func_00123ED8` is missing an `else` when claiming a two-block pool pair

The entity pool function that claims two neighbouring blocks for a large
entity type decides whether the second block can be used like this:

```c
if (next kind is FREE or READY) {
    second_free = TRUE;
}
if (next kind is EXTENSION && first_free) {
    second_free = TRUE;
} else {
    second_free = !func_00124008(pool_size + 1);
}
```

The first `if` has no `else`, so its result is always overwritten by the
second. The compiler kept the first test as dead code, which is how it shows
in the assembly.

Effect: a second block that is free or ready is not accepted outright; it goes
through `func_00124008` like any other block. Whether that changes the outcome
depends on what `func_00124008` returns for a free or ready block, which has
not been checked. Note the first block's test a few lines above is written
correctly, with the `else`.

## `func_001175F8` sizes a scrolling texture's sprite from its width only

The function that redraws a map's scrolling textures into their target
textures sets the frame, scissor and offset from both `target_width` and
`target_height`, then builds the sprite it draws from the width alone:

```c
half = texture->target_width >> 1;
quad.y0 = quad.x0 = (0x800 - half) << 4;
quad.y1 = quad.x1 = (half << 4) + 0x7FF8;
```

Effect (a guess): correct for square targets, wrong height for any other.
Possibly every scrolling texture in the game data is square; not checked.

## `func_0010CB98` tests a 7-bit parent index against -1

`ModelPart` keeps its parent part's index in a 7-bit field, and
`func_0010B758` stores "no parent" there as an `s8` -1, which the field holds
as 0x7F. `func_0010CB98` (project a model's vertices to screen-space texture
coordinates) tests the field against -1, which can never be equal, so a part
with no parent is treated as having parent 0x7F:

    parent matrix = scratch->matrices[0x7F]

That is 0x1FC0 bytes into the scratchpad, past the 0x40 matrices it holds.

Effect (a guess): none in practice. The parent matrix is only used for
vertices whose "on parent" word is set, and root parts probably have none.
A port that bounds-checks the scratch array will trip here.

The draw functions `func_0010BA30` and `func_0010C180` have the same test.
There a root part still gets a "parent" packet, built from
`part_matrices[0x7F]` and `part_turns[0x7F]`; the second is scratchpad memory
these functions never write, so stale data is sent to VU1 for root parts
(presumably ignored by the microprogram; a guess).

## `func_0010E010` sends unwritten packet data and restores the wrong Z buffer register

`func_0010E010` is one of the model draw functions. Two slips:

- **Packet length.** When the model has `unk100 & 0x800` set, it allocates
  0x21 quadwords for the header packet but writes 0x1C either way. The end of
  the packet handed to `func_00109650` is the address of the next allocation,
  so the last 5 quadwords go to VIF1 unwritten.
  Effect (a guess): whatever the packet buffer held two frames earlier is sent
  along; harmless only if that happens to be zeros or ignored data.
- **Z buffer restore.** After the parts loop it writes `ZBUF_2 = 0x10A000118`
  twice in a row and never restores `ZBUF_1`, which stays as the last opaque
  part left it (z writes on or off). One of the two writes was probably meant
  to be `ZBUF_1` (a guess).

Also in this function, with no effect: in the opaque path `ZBUF_1 = 0xA000118`
is written and then written again at once by the `if` that follows.

Its sibling draw functions do not share either slip: `func_0010D308`,
`func_0010D700` and `func_0010DB40` write every quadword they allocate, and
`func_0010C180` restores both Z buffer registers. `func_0010DB40` never
restores `ZBUF_1` after its last part, which may or may not be intended.

## `func_001108E8` / `func_00110D60` multiply where they should subtract

These two interpolate the vertices between two ends when a triangle is
subdivided (`func_001108E8` takes packed s16 vertices, `func_00110D60` floats).
Every component's per-step change is `(last - from) * scale`, except the two
texture values in `unk20[]`, where it is `last.unk20[n] * from.unk20[n] * scale`:
a product.

Effect (a guess): the texture coordinates of the in-between vertices of a
subdivided triangle are wrong unless the product happens to land near the
difference. Both are marked with `// match:` comments.

## `func_00114AA8` can lose its place in a map file

`func_00114AA8` builds a map's models. Its loops over the map file's plain
models, jointed models and motions set `next = cursor` only inside the "some
object uses this entry" test, but do `cursor = next` unconditionally
afterwards (marked with `// match:` comments). The collision-mesh loop has a
related weakness: its cursor only advances through the return value of
`func_00112400`, which is called only for the first object that uses mesh `i`,
so an unused mesh leaves the cursor where it was.

Effect (a guess): if no object uses entry `i`, the cursor is not advanced past
it and later entries are read from the wrong place; if that is the very first
entry, `next` is read uninitialised. Harmless as long as every entry in every
map file is used by at least one object, which is presumably true of the
shipped data. A port that loads modified maps should not rely on it.

## Smaller oddities

Things that look unintended but have no known effect. Kept here so a port
does not mistake them for meaning something.

- `func_0011A600` (map viewer load) sets the pointer model's scale with a
  chained assignment that also writes `rotation.w = 4.0f`. Every other
  placement sets `rotation.w` to 1.0 or 0. Nothing is known to read it.
- `func_0011B1C0` (player spot on a map picture): on picture 1, lower level,
  map tag 0x61 has its own `case` but returns FALSE like the default, while on
  the upper level tags 0x5F-0x61 all get a spot. Either map 0x61 has no lower
  floor or the case was left unfinished; the effect is no marker there.
- `func_00112648` (data table load) converts a field of each of the 380 item
  definitions with `unk3E = unk33 * 2.2046`, kilograms to pounds. Not a bug:
  it looks like a localisation step, and tells you `unk33` is a weight.
- `func_0011D180` (update all map objects) calls
  `sinf(D_004467A0 * 2pi / 60)` every frame from a 60-frame counter and throws
  the result away. The counter is still advanced. Probably a leftover pulse
  value; the only effect is the wasted call.
- `func_0011C708` (set up all map objects) switches on a one-bit field and has
  a `default` arm that sets `model->draw = NULL`. It can never run.
- `func_0010FC30` (interpolate vertices, rounding) is not called from anywhere,
  and fills two stack arrays it never reads back.
- `func_00104EC8` (save loader; not matched yet, read from the assembly) copies
  the save's magic word (0x15400) and stored size into locals and never checks
  them. Whether its caller validates the buffer first is not known. Its last
  loop also reads one byte past the final flag byte; the buffer is 0x20000
  bytes, so nothing comes of it, and it may be the compiler's read-ahead, not
  the source.
