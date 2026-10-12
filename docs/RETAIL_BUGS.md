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

## `func_001A1E88` tests the address of the slot pool where it means to stop early

`func_001A1E88` runs the effect spawner slots and the effects for one frame.
Its first loop walks the 0x60 slots of `D_003F1750`, and after running one:

```c
func_001A24D8(slot);
if (!D_003F1750) {
    break;
}
```

`D_003F1750` is the pool array itself, so the test is on its address. Retail
has it as `addiu $2, $18, %lo(D_003F1750)` / `bnez $2`.

Effect: the `break` never runs and the loop always walks all 0x60 slots. The
loop is only entered when the live count `D_004468CC` is above 0, and the slot
loop of `func_001A3040` stops on that count, so the count was probably what
was meant here (a guess). Marked with `// match:`.

## `func_001A3040` takes one record too many for the map it unloads

`func_001A3040` frees the effects of a map slot that is going away and clears
what the other effects still point at in it. For the map's entities and map
objects it builds a range from the first to the last element and tests
`>= first && <= last`. For the map's records the end is one past the last:

```c
char *last_record = first_record + map_slot->header->record_count * 0x80;
...
if (effect->unk58 != NULL && effect->unk58 >= first_record && effect->unk58 <= last_record) {
    effect->unk58 = NULL;
}
```

Effect: an effect whose `unk58` is exactly the address after the map's last
record has it cleared too. Whether anything can sit there (the other slot's
first record, say) has not been checked. Marked with `// match:`.

## `func_001B29C8` skips the player's first defence and drops the adjusted attack

`func_001B29C8` works out the damage of a hit on the player: nine terms, each
one of the attack's nine strengths against one of the player's nine defences
(`PlayerState.unk90.values[]`). Two slips:

- **Defence index.** The defences used are `values[1], [1], [2], [3], ... [8]`:

  ```c
  damage = func_001B4AA8(request->unk50.unk00[0], D_00413F80.unk90.values[1], D_00413F80.unkAC);
  damage += func_001B4AA8(request->unk50.unk00[1], D_00413F80.unk90.values[1], D_00413F80.unkAC);
  damage += func_001B4AA8(request->unk50.unk00[2], D_00413F80.unk90.values[2], D_00413F80.unkAC);
  ```

  `values[0]` is never read here, though `func_001BA310` fills it like the
  other eight.
  Effect: the player's first defence does nothing against a hit, and the
  second counts against the first two strengths.
- **Adjusted attack.** `func_001B3ED8(&attack, request, extra)` fills a local
  block of nine strengths, and `attack` is never read; the terms use
  `request->unk50` as it came. The two sibling functions, `func_001B2F70` (hit
  on an entity) and `func_001B3AC0` (hit on a map object), make the same call
  and do use `attack.unk00[n]`.
  Effect (a guess): whatever `func_001B3ED8` adds on the attacker's side does
  not apply to hits on the player.

Both are marked with `// match:` comments.

## `func_001B70F8` sets the first magic's flag for the second magic

A swung weapon can carry two magics. Near its end `func_001B70F8` has two
blocks of the same shape, one per magic; the second tests bit 2 of the byte at
`PlayerState` 0x260 and then sets bit 1:

```c
if (D_00413F80.unk25F != 0 && !D_00413F80.unk260_1 && def->unk46 != 0xFFFF && ... >= def->unk48) {
    D_00413F80.unk260_1 = 1;
}
if (D_00413F80.unk25E != 0 && !D_00413F80.unk260_2 && def->unk56 != 0xFFFF && ... >= def->unk58) {
    D_00413F80.unk260_1 = 1;
}
```

Effect: bit 2 is never set in this function, so its test is always true there
and the second magic marks the first magic's bit. What reads the two bits has
not been found, so what this changes in play is unknown.

## `func_001BC9E0` / `func_001BD208` work out a shot's angles and pass the player's

Both launch a magic shot. Each copies the player's rotation into a local, adds
`PlayerState.unk238` to its x and clamps that to +-1.012291 rad (58 degrees),
and then passes the player's own rotation to `func_001B4F30`:

```c
func_00216778(&angles, &D_00413F80.rotation);
angles.x += D_00413F80.unk238;
/* clamp angles.x */
func_001B4F30(magic_id, shot, &D_00413F80, &D_00413F80.rotation, &D_00413F80);
```

`angles` is not read again.

Effect (a guess): shots leave along the player's plain rotation, without the
`unk238` offset and without the 58 degree limit. Both are marked with
`// match:`.

## `func_001BB908` / `func_00100D10` shift a record's byte offset by 14

Both look up the map record a point is in with `func_00115CD0` and store its
index in `PlayerState.map_index`:

```c
player->map_index = (u32)((char *)record - D_0038CD20[D_00446770].records) >> 14;
```

The records are 0x80 bytes each, and every reader of `map_index` uses it as a
record number (`records[map_index]`, `map_index << 7`,
`map_index < record_count`). A shift of 7 would give that number; 14 gives the
number divided by 128.

Effect: these two store 0 for any of the first 128 records. `unk250`, set on
the next line from the record itself, is right. `func_001BB908` only runs when
`map_index` is 0xFFFF (from `func_001B5A48`); in normal play the floor test in
`func_001BDDC0` sets the index, correctly. So the player is probably taken to
be in record 0 for as long as no floor is found under them (a guess; not
checked how long that lasts or what `func_00100D10`'s mode is).

## `func_001BFCC8` copies as many parts as the larger of two models has

When it copies the pose of the item model (`D_0057B6A0`) to the arm's model
(`D_0057B860`), part by part, the loop count is the larger of the two part
counts:

```c
count = D_0057B6A0.part_count > D_0057B860.part_count ? D_0057B6A0.part_count : D_0057B860.part_count;
```

Effect: if the counts differ it reads past the end of one `parts` array and
writes past the end of the other. Probably never seen, the two models being
made on the same skeleton (a guess; the data has not been checked).

## `func_001C1C80` widens a record's box by one unit, not by the ray's length

`func_001C1C80` tests a ray from `position` to `target` against the map. Like
its siblings `func_001C1430` and `func_001C1858`, it skips a map record unless
the start point is inside the record's box, widened backwards along the ray.
The siblings widen by `step = direction * scale`, the whole way. This one
normalises `direction` first and widens by that, so by a vector of length 1;
the length is kept apart and only passed to the triangle test.

Effect (a guess): a record is tested only when the ray starts inside its box
or within one unit of it, so a ray that starts in one record and ends in a
wall of the next misses that wall. The only caller, `func_001B1B90`, uses it
for the two edges the player's weapon moved through, so this would be weapon
swings that fail to strike a wall.

## `func_001C6360` does not shift its side notes between its two triangles

`func_001C6360` tests a fan against a face of two triangles (strip points
0-1-2 and 1-2-3) and leaves a triangle out when all three of its points lie
outside one edge of the fan. It collects one bit per point, shifting left
after points 0 and 1, and compares with 7. Inside the loop over points 2 and 3
there is no shift and no mask, where the three-point version `func_001C5280`
does `sides = (sides << 1) & 7` each step.

Effect: for the second triangle the value compared with 7 is
`p0 << 2 | p1 << 1 | (p2 | p3)`. It is left out when points 0 and 1 and
either of 2 and 3 are outside, where points 1, 2 and 3 were meant. The second
triangle is sometimes skipped when it should be tested, and the other way
round.

## `func_001CBED8` / `func_001CC018` only step to the next voice after a reload

These two switch every loaded voice and bank to its other header block. Their
loops count to 16 (voices) and 4 (banks), but the pointer is advanced at the
bottom of the body, after tests that `continue`:

```c
for (i = 0; i < 16; i++) {
    voice->unk1C = 0;
    if (voice->unk00 == 0) {
        continue;
    }
    ...
    func_001CBAE0(1, 0x1010, voice->unk18, voice->unk0C, voice->unk04, voice->unk1B);
    voice++;
}
```

Effect: the loop stays on the first voice (or bank) that fails a test for all
the turns that are left. Voices stored after a free slot are not switched and
keep their old `unk1C`; if voice 0 is free, none is. What that sounds like has
not been checked.

## `func_001CD3D0` keeps the voice when it fails

`func_001CD3D0` sets up a free voice from data already on the IOP. It stores
`voice->unk00 = header_block` early, which is what marks the voice as taken,
and allocates an IOP block for `unk04`. Every failure after that is a bare
`return -1`. The other loaders (`func_001CC1A8`, `func_001CC400`,
`func_001CC6A0`, `func_001CC888`) call `func_001CD6E0(voice_index)` to release
the voice before they return -1.

Effect: after a failed call the voice stays taken and its `unk04` block stays
allocated. Not checked whether any caller can make it fail. `func_001CC1A8`
has one such bare return too (when `func_001CF110` fails). Each of these
returns is marked with `// match:`.

## `func_001CFFA0` fills its table of glyph alphas a byte at a time

`D_005C50C8` holds one `int` of alpha per glyph of a message, 0x80 of them.
The typing branch of `func_001CFFA0` writes it as ints
(`D_005C50C8[i] = 0x80`), and the draw loop reads it as ints. The ring branch
and the box's closing branch fill it with

```c
memset(D_005C50C8, alpha, 0x200);
```

so each entry becomes `alpha * 0x01010101`.

Effect: `func_001D0298` is handed that word as the glyph's alpha. Unknown what
it does with the upper bytes (it is not matched yet); nothing is seen if it
only uses the low byte.

Also here: nothing bounds the index. The typing branch writes
`D_005C50C8[shown + i]` for `i < 8` and the draw loop reads one entry per
glyph, so a message of more than about 0x78 glyphs would run past the table.
Not checked whether any message is that long.

## `func_001B7EE0` tests the looking speed where it slows the turn

When the player lets go of the turn buttons the turning speed `unk1BC` is
brought back to 0 by `turn_loss` a frame and clamped at 0. There are four such
blocks (looking speed `unk1B8` and turning speed, each from above and from
below). Three test the field they just changed. The fourth tests the other
one:

```c
if (D_00413F80.unk1BC > 0.0f) {
    D_00413F80.unk1BC -= turn_loss;
    if (D_00413F80.unk1B8 < 0.0f) {
        D_00413F80.unk1BC = 0.0f;
    }
}
```

Effect: while the looking speed is negative, a positive turn stops at once
when the button is let go. Otherwise an overshoot below 0 is not clamped here,
but the block after it (`unk1BC < 0`: add the loss, clamp at 0) puts it right
in the same frame.

## `func_001D3730` reads `next` of the spare upload node without a NULL test

`func_001D3730` copies the current upload node into the spare node
`D_005CE8B0` and cuts the chain after it. When the current node already is the
spare one it does

```c
if (node->next->callback != node->callback) {
    return;
}
```

The other branch of the same function tests `D_005CE8B0.next != NULL` first,
and that branch is also the one that sets the spare node's `next` to NULL.

Effect: with `next` NULL it reads the word at address 0x18. On the PS2 that is
readable, so the compare most likely just fails and the function returns (a
guess). A port must not dereference there.

## `func_001D43B8` carries on after its file fails to open

Stage 0 of a chunk upload. When the open fails it clears `stage` and `done`,
sets its own `upload` pointer to NULL, and then falls into the code for the
success case:

```c
if (file_handle < 0) {
    upload->stage = 0;
    upload->done = 0;
    upload = NULL;
}
func_001025C0(upload->unk08, upload->pool_node->buffer, 0x1A2000);
D_00446AD0 = upload->unk08;
upload->stage++;
```

Effect: on a missing file it reads a handle and a buffer pointer from low
memory, starts a read with them and increments the word at address 0x1C. A
`return` was probably meant. Its sibling `func_001D3F78` opens a fallback file
at that point (and does not check that one either).

## `func_001DD738` / `func_001DCA80` leave `D_00426470` pointing at a stack local

Both draw a model by filling in the global link node `D_00426470` and handing
it to `func_00201E10`. The node's `pool_node` is set to a `ChunkPoolNode` that
is a local of the function, and `unk20` to 1:

```c
D_00426470.pool_node = &model_node;
D_00426470.unk20 = 1;
model_node.buffer = chunk;
func_00201E10(&D_00426470, local, D_00446C78);
```

Effect: the draw itself is fine, the local is alive during the call. After the
return the global still holds the address and still says "loaded".
`func_001D6E60` draws the same node for other screens; not checked whether
`pool_node` is always set again before that happens. Same kind of slip as the
`Entity.target` entry at the top.

## `func_001EEF78` indexes a two-entry handler table with three menu lines

When its fade-out ends, the screen with three lines does, for whichever line
the cursor is on (`context->unk04`, 0 to 2):

```c
D_00446CA0 = D_004402F8[context->unk04];
```

`D_004402F8` has two entries (`func_001E7010`, `func_001EA568`). For line 2
the word read is the one after the table, the first pointer of the data block
`D_00440300`.

Effect: on the third line `D_00446CA0`, a handler that `func_001E68D0` calls,
is set to the address of data. Probably never called in that state, since the
third line opens the other list (`func_001EEE80`, through `D_00446CC4`); not
checked.

## `func_001D8C90` clamps the text alpha where it means the cursor bar's

In the magic menu's fade-in the cursor bar's alpha `D_00425510[253]` is raised
by 1.6 a frame; the clamp that follows tests it and then writes another entry:

```c
D_00425510[253] += 1.6f;
D_00446C78.alpha = (s32)D_00425510[255];
if (D_00425510[253] > 16.0f) {
    D_00425510[255] = 16.0f;
}
```

`[255]` is the text's alpha. The item menu `func_001D8398` clamps `[253]`
itself at the same place.

Effect: during the fade-in the bar's alpha is not held at 16, and once it
passes 16 the text alpha is put back to 16 every frame, after it has been
copied to the colour. What that looks like is a guess: a text fade-in that
stalls until the fade-in state ends.

## `func_001D94F8` resets the accessory panel's alphas from the armour panel's list

In the equipment screen's detail panel, the case for groups 7 to 9 draws and
fades the parts of `D_00447268` (count `D_00446A7C`), but when the panel has
just opened it fills the alphas from the other list:

```c
if (D_004472C7 == 1) {
    RESET_ALPHAS(D_00447240, D_00446A44);
}
DRAW_PARTS(D_00447268, D_00446A7C, D_00446E1C);
```

The case for groups 2 to 6 uses `D_00447240` for both.

Effect (a guess): parts of the accessory panel start at another panel's
alphas, and if the armour list is the shorter one the last parts are not reset
at all. Nothing is seen if the two lists match part for part; not checked.

## The item detail panel's switch leaves its part list unset for groups 1 and 10

Six screens open a detail panel for the current item with the same code: a
`switch` on the item's group sets `packets` and `count` (the panel's sprite
list), and the fade code right after the switch walks `packets[0..count)` and
writes `D_00425910[i]`:

| Function | Groups with no case |
| --- | --- |
| `func_001E7010`, `func_001E88A0`, `func_001EA568`, `func_001EBB98`, `func_001EF840` | 1 and 10 |
| `func_001D94F8` | 10 |

For a group with no case both variables are read unset; in retail they are
saved registers the function has not written yet.

Effect: `func_001E4FE0` decides whether the button opens the panel. It refuses
group 10 and accepts group 1 for any record but 0, so in the five trade
screens the fade can run over a stale pointer and count whenever a group 1
item is current. What then happens depends on the caller's registers (a
guess: usually a count of 0 or a harmless read; a large count would write
past `D_00425910[256]`). Not checked which of these lists can hold group 1
items; `func_001D94F8` has its own case for group 1.

## `func_002050F8` clamps an unsigned Y at 0

`func_002050F8` copies a sprite packet and moves the copy up:

```c
D_005DD640.xyz0.Y -= offset;
D_005DD640.xyz1.Y -= offset;
if (D_005DD640.xyz0.Y < 0) {
    D_005DD640.xyz0.Y = 0;
}
```

`Y` is an unsigned 16-bit field, so neither clamp can run; the compiler
removed them and only their stack frame is left.

Effect: an offset larger than Y wraps round to a large Y. Not checked whether
any caller moves a sprite that far.

## `func_00207D68` range-checks an ini item number too late and with the wrong base

The handler of the `item_NNN=count,level,time` key of the start-up ini file.
Two slips:

- `D_00369830[item] = atoi(text)` is done for any three digits, before any
  range test. The table has 380 entries.
  Effect: `item_380` and up write a byte past the table. Only reachable
  through the ini file.
- The level of items 220 and up goes to `D_003682F0[item - 220]`, which has
  160 entries, but the test in front of it is `(u32)(item - 160) < 160`.
  Effect: items 320 to 379 never get their level from the ini. (160 to 219
  pass the test too, but the weapon branch before it has taken them.)

## `func_0020B278` clears only one of the bytes left over

`func_0020B278` clears a buffer eight bytes at a time and then the remaining
0 to 7 bytes:

```c
remaining = byte_count & 7;
while (remaining--) {
    *dest = 0;
}
```

`dest` is not stepped.

Effect: when the size is not a multiple of 8, the first leftover byte is
cleared and up to six after it keep their contents. No reference to the
function was found in the source or the remaining assembly, so it may never
run.

## `func_0020FCF8` takes the height of a swing's hit from a local that is never set

`func_0020FCF8` tests a swing's arc, with pitch, against a cylinder. At the
end it clamps `level` between the first cylinder's bottom and top and stores
it as `CollisionResult.point[1]`. Nothing assigns `level`; retail reads `$f31`
as the caller left it.

Effect: the y of the hit point is some leftover float, held to the first
cylinder's height range. Whatever uses that y (not traced) gets a height that
has nothing to do with the other cylinder, which is what was probably meant.

## `func_0020FA58` divides two levels where it means their middle

The same test without pitch. The y of the hit point is set in four overlap
cases, each of this shape:

```c
COLLISION_RESULT->point[1] = other_top / other_bottom * 0.5f;
```

and likewise `top / bottom`, `bottom / other_top`, `top / other_bottom`.

Effect: the stored y is half the ratio of two heights, so about 0.5 whatever
the heights are, and very large when the divisor is near 0. `(a + b) * 0.5f`
was probably meant. The x and z of the point are right.

## `func_0020DC68` builds a capsule's far end from the axis alone

`func_0020DC68` tests a point against a capsule (`base`, unit `axis`,
`length`). Past the far end it works out that end as

```c
KF_Vector_Scale(offset, axis, length);
func_00216C48(offset, axis, offset);
```

that is `axis + axis * length`. The near end uses `base`.

Effect: the far cap is tested round a point within `length + 1` of the world's
origin, so in practice it never hits there. The code then falls to the side
test, which has no limit along the axis, so a point past the far end hits
whenever it is within reach of the capsule's axis line carried on.

## `func_0020EBB8` tests "parallel to the plane" with a sum of components

`func_0020EBB8` tests a move against a triangle's plane. Before dividing by
`plane . move` it means to leave when the move is parallel to the plane:

```c
func_00216C48(edge_a, plane, direction);
facing = edge_a[0] + edge_a[1] + edge_a[2];
if (-0.00001f < facing && facing < 0.00001f) {
    return;
}
```

That adds the two vectors and sums the result's components; it is not a dot
product.

Effect: a move is thrown out when `nx + ny + nz + dx + dy + dz` is within
0.00001 of 0, which has nothing to do with being parallel, and a move that is
parallel goes on to a division by (nearly) 0. Also here, with no effect: the
two edges are worked out in VU0 code and then again with `KF_Vector_Subtract`
over the same locals.

## `func_00210F88` masks a script command number with 0x7FFFF and then does not use the mask

`func_00210F88` steps the commands of a script thread whose number has bit
0x8000 set:

```c
index = number & 0x7FFFF;
if (index >= 0xC8) {
    D_00433650[number - 0xC8](&thread->commands[i]);
```

The mask keeps bit 0x8000 (0x7FFF was probably meant), and the three handler
tables are indexed with `number`, not `index`.

Effect: any such command calls through `D_00433650[number - 0xC8]`, at least
0x7F38 entries past the start of the table. `func_00211C38` stores the number
from the upper 16 bits of a script word, so a script can produce one;
presumably no shipped script does, or the game would crash (a guess).

## `func_002109C8` keeps the thread when the script is not found

`func_002109C8` starts a script by id. It claims a free thread, stores
`script_id` and the cursor in it, and then looks for the script's label. When
it reaches the end label (0x7FFE) first:

```c
if (operand == 0x7FFE) {
    return 0;
}
```

Effect: the thread stays claimed with its cursor on the end label and its
commands not cleared, and since `func_00210C48` goes by `script_id` the id
counts as running from then on. What the per-frame update does with such a
thread is not checked. The 0 returned is also what "already running", "no free
thread" and "started in thread 0" return.

`func_00210AF8`, the version that starts a script even if it is running, has
no end-label test at all: an id that is not in the data makes the search read
on past the end of the script data.

## `func_00214B88` has no `break` in its blend mode switch

The script command that sets the screen overlay's colour and blend mode:

```c
switch (command->blend.value) {
case 0:
    overlays->unk80 = 0x44;
case 1:
    D_0038A1B0.unk80 = 0x48;
case 2:
    D_0038A1B0.unk80 = 0x42;
default:
    D_0038A1B0.unk80 = 0x44;
}
```

Effect: `unk80` is 0x44 whatever the script passes; 0x48 and 0x42 never take
effect. Retail has all four stores in a row.

## `func_0021B3C0` overwrites its own "card full" message

```c
free_clusters = MC_CARD_INFO->free_clusters[port][slot] - 2;
if (free_clusters <= 0) {
    D_01E4F278.timeout = 0x1E;
    free_clusters = 0;
}
D_01E4F278.timeout = 0;
```

Effect: the 0x1E is never seen; a caller only learns of a full card from the
0 returned. The sibling `func_0021B478` keeps its 0x1E.

## `func_00219CD8` stays in its error step for ever

`func_00219CD8` steps the "copy a file to the card" request. Step 100 is where
an error after the open goes: it closes the source file, and when the card
file is open it should move on to step 101, which closes that one.

```c
case 100:
    if (D_01E4F264[0] != 0) {
        sceClose(D_01E4F264[0]);
    }
    if (D_01E4F334.file_fd < 0) {
        ...
        return -999;
    }
    D_01E4F278.unk9C = 0;
    break;
```

The `D_01E4F278.unk8C++` that `func_00219700` and `func_0021A368` have at this
place is missing.

Effect: once in step 100 with the card file open, the request returns 0
("running") every frame for good: it calls `sceClose` on the same source
handle each time, never closes the card file and never reports the error.
Probably a hang of whatever screen waits for the request (a guess; it needs a
card error during the copy).

## `func_00218F68` reads an empty last block for ever

`func_00218F68` reads a checked file from the card in blocks of 0x3FF data
bytes (0x3FB in the first) and stops when a block is shorter than that. In
step 14 the length of the block just read is `chunk = size - offset`, and

```c
if (chunk != 0) {
    ... /* check, copy; a short block ends the read */
}
D_01E4F24C[0] += chunk;
D_01E4F278.unk9C = 0;
D_01E4F278.unk8C--;
```

Effect: when the data ends exactly on a block boundary (0x3FB + n * 0x3FF
bytes) the last full block does not end the read, the next block has length
0, and the request then goes round steps 13 and 14 without ever finishing.
Not checked whether any file the game reads this way has such a length.

## `func_0021A368` reads on when it has enough and stops when it has not

`func_0021A368` reads a card file as it is, 0x400 bytes at a time, into the
caller's buffer of `unkA8` bytes. Step 6, after a block has arrived:

```c
if ((u32)D_01E4F278.unkA8 < D_01E4F274[0] + result) {
    result = D_01E4F278.unkA8 - D_01E4F274[0];
    memcpy((u8 *)D_01E4F278.unkB4 + D_01E4F274[0], D_01E4F338, result);
    D_01E4F278.unk8C--;
} else {
    memcpy((u8 *)D_01E4F278.unkB4 + D_01E4F274[0], D_01E4F338, result);
    D_01E4F274[0] += result;
    D_01E4F278.unk8C++;
}
```

The two arms look swapped: when the block would overrun the buffer it copies
the part that fits and goes back to read another block, without moving the
offset; when there is room it copies the block and goes on to close.

Effect: at most one block is ever delivered. With a buffer of 0x400 bytes or
more the caller gets the first block of the file and no more; with a smaller
one the start of the buffer is overwritten by each later block until the file
runs out. Right only for files of up to 0x400 bytes read into a buffer at
least as large. Not checked what the game reads with it.

## `func_002186E0` / `func_0021A368` mark the request idle before testing the close

The last step of both (23 and 9) waits for the card file's close:

```c
if (sceMcSync(1, NULL, &result) != 0) {
    D_01E4F278.state = 0;
    if (result < 0) {
        if (D_01E4F278.unk9C == 2) {
            ...
            return -8;
        }
        D_01E4F278.unk9C++;
        D_01E4F278.unk8C--;
    } else {
```

`state = 0` marks the request as finished. `func_00219CD8` clears it only in
the two arms that return.

Effect (a guess): when the close fails with tries left, the step is put back
for another try and 0 ("running") is returned, but the request is already
idle, so the retry probably never runs and the failed close is not reported.

## `func_0021D208` returns with the stream's semaphore held

`func_0021D208` (the movie player's "add to the DMA chain", `viBufAddDMA` in
Sony's sample) takes `stream->sema` and then:

```c
if (!stream->is_active) {
    func_0021BDD8(D_00442128);
    return FALSE;
}
```

There is no `SignalSema` on that path. `func_0021BDD8` is an empty function in
retail, so the message is not shown either.

Effect: after one call with the stream not active, every later
`WaitSema(stream->sema)` blocks for good. Probably never reached in the game
(a guess). The slip is in the sample code as shipped.

## `func_0022AEF0` (libdev) stores VU0's vector registers where it loads the rest

This one and the next are in Sony's libdev, not in the game's own code, and no
call to either was found in the game.

`func_0022AEF0` is the "put" half of a pair over a VU0 register block. It
loads status, MAC, clipping, R, I, Q and VI0 to VI15 from the block with
`ctc2`, but before that it runs

```c
__asm__ __volatile__("sqc2 $vf0, 0x0(%0)" : : "r"(vectors));
```

for all of VF0 to VF31: `sqc2` stores. The "get" half, `func_0022AE10`, reads
the same control and integer registers into the block and does not touch the
vector registers.

Effect: the vector registers are never set from a block; "put" overwrites the
block's first 0x200 bytes with their current values. Either `lqc2` was meant,
or the run belongs in the other function (a guess). Marked with `// match:`.

## `func_0022B888` (libdev) tests VIF1's "FIFO full" bit on VIF0

Sony's libdev again. `func_0022B888` writes quadwords to the VIF0 FIFO and
`func_0022BC10` to the VIF1 FIFO; both stop on the same bit of their status
register:

```c
if (LIBDEV_REG32(VIF0_STAT) & 0x10000000) {
    return written;
}
```

Bit 28 is the top of VIF1's five-bit FIFO count (16 quadwords: full). VIF0's
FIFO holds 8 and its count is the four bits 24 to 27, so bit 28 of `VIF0_STAT`
is never set.

Effect: the VIF0 version never stops early; it writes every quadword whether
or not the FIFO has room. Marked with `// match:`.

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
- `func_001B29C8` (hit on the player) returns early unless `request->unk72` is
  0, then uses that same value in `if (damage <= blocked) damage = 1;`. It
  works as "at least 1 damage" only because the value is known to be 0 there.
- `func_001B21F8`, `func_001B24B0`, `func_001B2740` (hit sphere against a map
  model's strips) return no value when the model has no mesh or fewer than
  four points, and the first and last also when no triangle is touched. The
  caller `func_001B1510` only uses the value when the scratchpad says there was
  a hit. `func_001C8B58` (place a fixed item) and `func_001CB2C8` (gold from an
  entity) likewise return nothing on one path each; `func_001C8B58`'s is its
  success path, and `func_001C8C80` passes that value on.
- `func_001B1B90` (swept attack) builds a point from the request's position
  with `unk34` added to y before its map object loop and never uses it; the
  objects are tested with the weapon's edge points only.
- `func_001BDDC0` copies the player's position, adds 3.0 to its y, and never
  uses the result.
- `func_001B0390`, `func_001B04F0`, `func_001B0658`, `func_001B07C0` read the
  pad through `D_00447108`, a pointer nothing in the matched code sets. They
  look like debug leftovers; called as they are they would read near address 0.
- `func_001B1470` (run an attack request by shape): shapes 1 and 2 go to
  `func_001B1B80` and `func_001B1B88`, which only return 0, so a request of
  those shapes never hits anything.
- `func_001B2F70` (hit on an entity) tests
  `owner == player && (owner is outside the entity table)`. The second half is
  always true for the player. It also has two live `printf`s for the last
  boss (`"g_dmgLassBossCnt[%d]\n"` and an "HP halved" line), and
  `func_001C4498` (sphere against a pitfall object) calls
  `puts("Hit HitSpherePitfallObj()")` on every hit. Debug output left in.
- `func_001BA310` (work out the player's derived numbers): inside
  `if (unk3D != 0)` retail has only the divide checks of three divisions, by 3,
  5 and 15, whose results were never used. What was divided cannot be told
  from the binary; the C divides `unk20` as a stand-in. Whatever `unk3D` was
  meant to change here, it changes nothing.
- `func_001C0A28` (frames left of a trail point) accepts `index <= count`,
  one past the newest point; its sibling `func_001C08A0` tests `index < count`.
  The one caller found, `func_001C0268`, loops `i < count`.
- `func_001C1858` (ray against the map, with a mask of strip lists) walks
  seven strip lists per mesh. `CollisionMesh` names six and no other function
  seen uses a seventh. No caller is matched yet, so whether bit 6 of the mask
  is ever set is unknown.
- Map objects of kind 2 are tested as one box (points 0, 1, 6, 7) in place of
  two while animation 4 is on frame 0 or 1 by `func_001C3FA0` (sphere) and
  `func_001C58F0` (fan), but only on frame 1 by `func_001C31E8` (ray) and
  `func_001C6B88` (circle with push). May be intended.
- `func_001C2F30` and `func_001C31E8` (ray against kind 1 and kind 2 objects)
  and the two-quad branch of `func_001C6B88` use what `get_point()` returns
  without a NULL test. The sphere versions (`func_001C3DF8`, `func_001C3FA0`)
  and the other branch of `func_001C6B88` return on any NULL point. No effect
  as long as every such model has its points.
- `func_001C5278`, `func_001C65E8`, `func_001C7AF0` (the pitfall case of the
  point-with-two-reaches, fan and circle tests) are empty functions, called
  with full arguments. Those shapes never hit a pitfall object; the plain
  sphere and the ray do.
- `func_001CB0D0` (drop gold) stores `COLLISION_RESULT->point[1]` in
  `actor->unk14` whether or not it ran the ground test (`on_ground`). Without
  the test that is whatever the last collision test left. In this function the
  value is only used when `on_ground` is set; `func_001CACF8` later adds a map
  offset to it.
- `func_001CBAE0` (the game's copy of `sceSdRemote`) copies eight argument
  words into the request. The register save area holds six after the command,
  so the last two come from the caller's frame. No command seen uses them.
- The voice loaders `func_001CC1A8`, `func_001CC400`, `func_001CC6A0`,
  `func_001CC888` free `voice->unk00` if it is not 0, right after picking the
  voice because its `unk00` is 0. That free can never run.
- Voice ids: `func_001CC888` and `func_001CD3D0` reject `>= 17`,
  `func_001CC400` and `func_001CC6A0` reject `>= 16`, `func_001CC1A8` does not
  check. The id is only handed to the driver here; effect unknown.
- Bank indices: `func_001CE468`, `func_001CE4D0`, `func_001CE510`,
  `func_001CE568`, `func_001CE5C0` reject `>= 5`, but `SoundState` has four
  banks and the functions that take a bank id reject `>= 4`. Index 4 would
  read the first four note slots (`unk240[0..3]`) as a bank. No caller is
  known to pass 4.
- `func_001CDF90` (load a bank in steps) returns -1 for a bad bank id with its
  step still 0. -1 is also what it returns while it is still working, so a
  caller that polls would call again. Its other failures return -2.
- `func_001CE9A0` (queue a 10-byte note command) has no `default` in its
  `switch (kind)`: for a kind other than 0, 1, 2 it writes the three header
  bytes and still adds 10 to the queue, sending seven bytes of whatever the
  buffer held. The callers seen pass 0.
- Locals that are set and never read: a `Vector4` cleared by `memset` in
  `func_001B5A48`, 16 bytes cleared in `func_001CD8C8`, and a vector
  {0, 1, 0, 1} copied from read-only data in `func_001B21F8`, `func_001B24B0`
  and `func_001B2740`.
- `func_001D4228` (last stage of a node's load) accepts slots below 13 and
  `D_00420DF0` has 13 entries, but the table of per-slot values it copies from
  `D_0043F720` has 12. Slot 12 would pass a byte of stack to `func_002032E0`.
  No node seen is given slot 12.
- `func_001D3F78` (open a chunk's file, or the fallback file) does not check
  that the fallback opened; a negative handle would go to the read.
- Tables copied to the stack and never read: a second table of key item
  numbers in `func_001D50C0`, the weekday names in `func_001D62C8`, the stock
  table in `func_001E7010` and `func_001EA568`, two tables in
  `func_001D94F8`. `func_002020A0` builds a unit matrix and copies a 16-byte
  constant it never uses.
- More functions that return no value on some path: `func_001D4598` and
  `func_001D6E60` (declared `int`, return nothing; no caller reads it),
  `func_001D7AD8` for a step outside 0 to 5, `func_001D62C8` for a menu number
  outside 0 to 5 (`func_001F91F8` passes 1 to 3).
- `func_001D62C8` (run the menu) reads the clock, puts its BCD bytes into
  `mktime` fields as they are, takes 1 off the day of the month, and throws
  the result of `mktime` away. Looks like the remains of a date display.
- `func_001DC2A8` (key item screen) passes a `RenderSlotArg` it never sets to
  `func_002020A0`, which never reads its second argument.
- `func_001E5210`, `func_001E53A8`, `func_002062A0` build the ALPHA_1 value in
  a local that is never initialised and set only its five fields, so the
  other bits sent to the GS are leftovers. In the first two a mode outside
  0 to 3 (0 to 2 in the second) sets no field at all. The GS ignores the
  unused bits.
- `func_001D0970` (what the player faces): for a plain map object it
  subtracts the same two vectors twice, once for the cone test and again for
  the distance.
- Dead statements kept because the match needs them: `item = D_00426120[0]`
  at the top of `func_001D8398`, and `word.f.mid16 += 0xB0; word.f.lo16 =
  0x7080;` in the list loop of `func_001D8C90`. Each is live in the other of
  the two menus.
- `func_001D94F8` (equipment detail panel) picks the magic panel's variant
  with `(index - 1) / 15` in one layout and `(index + 1) / 15` in the other
  (`FxTableEntry.unk102 != 0`). The magic menu uses `(number - 1) / 15`. May
  be intended.
- `func_001E0D98` and `func_001DF308` hand `func_001FC5E8` a cursor bar of
  which only the corners and the colour are set. `func_001FC5E8` (not matched;
  from its assembly) also reads the float at +0x24, so that one is stack
  contents. Presumably not visible.
- `func_001E0D98`: the last arm of the per-save `switch` is written as a
  fourth test, not an `else`, so retail keeps a path that sets neither
  argument of the call after it. The four tests cover every case.
- `func_001E2750` (button settings), on cancel: tests
  `has_duplicate && last_is_shared` and sets `D_00446BF0 = 1` in both arms.
- Menu functions that take no argument are called with a 0 all over the menu
  code (`func_001D45D0`, `func_001E4568`, `func_001E6928`, `func_001EEE80`,
  `func_001D50C0`).
- `func_001EBB98` passes `kind + 10000` to `func_00207478` where its two
  sibling screens pass `kind + 1000`. The callee takes the number modulo 1000
  and only tests `>= 1000`, so it draws the same.
- `func_00204580` (send a texture to the GS a strip at a time) tests
  `D_00420DC0 != NULL`, the address of a table of pointers, and then writes
  the texture's TEX0 over the first two pointers. Nothing calls it.
- `func_00201E10` and `func_00201F58` index a one-entry local table of texture
  pages with the same byte as a two-entry table of CLUT pages. The byte
  (`D_00446D80`, `D_00446D81`) is always 0.
- `func_00203820` (pose a skeleton node) reads `node->slot` before its
  `node != NULL` test and `node->unk38` after the NULL branch, so that branch
  can never have run. With a node that has no keys it uses the key pointers
  and the phase unset.
- `func_00205D80` copies the packet to `D_005DD940` like its siblings and
  then returns the packet it was given; the copy is not used.
- `func_002077F0` (read the start-up ini) does not stop at the first key that
  matches and replaces `line` by the text after `=`, so the remaining keys are
  compared with the value. A line that starts with a key but has no `=` hands
  `(char *)1` to the handler.
- The ini key for gauntlets is spelled `equip_guntlet=` (`func_00207BB8`).
- `func_00208BC0` (load the boot models): after setting up the second model
  it writes `D_00447394->unk1A8` again, the first model's. No effect seen:
  `func_00209018` sets `unk1A8` on the model it draws.
- `func_00209A48` (boot screen sprite) interpolates between key `i` and key
  `i + 1` of four; at the last key that reads one past both arrays. The ratio
  is 0 there, so the value is still the last key's.
- `func_0020D008` is not called from anywhere.
- `func_002110A0` (script reader), control word 0x19D: sets the cursor from
  `func_00211CF0`, a stub that returns 0, so the thread's cursor becomes NULL.
  Looks like a removed feature; presumably no script uses the word.
- `func_00210C88` (a thread's first free command) sets an error flag and
  returns 0 when all 16 are busy; `func_00211C38` then starts the new command
  over command 0.
- `func_002111F0` (the script's "if") stores up to 255 comparison results, by
  a count byte from the script, in a 32-entry stack array, and reads
  `results[0]` unset when the count is 0.
- `func_00210088` tests `side < 0.0f` twice in a row.
- The script variable readers and writers (`func_00211958`, `func_002119D8`,
  `func_00211AD0`, `func_00211B48`, `func_00211BC0`) set an error flag for an
  offset past the end and then read or write there anyway.
- `func_00211F60` and `func_00211F90` (two script commands, the same code)
  hand the script a value they never set: whatever `$5` held. They look like
  commands whose body was taken out. Not checked whether a script uses them.
- `func_00216D50` (set up an EE timer) steps 0x2000 bytes per timer where the
  timers' registers are 0x800 apart, so timers 1 to 3 would write to other
  hardware registers. The one call found passes timer 0.
- `func_0021A368` step 101: after the third failed close it clears the
  request but returns 0, where step 8 returns -1.
- `func_002186E0` step 100 tests `file_fd < 0` to skip the close. The
  descriptor is 0 or a result that was not negative, so the test is never
  true.
- `func_0021DFE8` (movie time stamps) takes its position from the decoder it
  is given and queues into the global `D_00437980`. There is one decoder, and
  it is that one.
- `func_00201518`, `func_002020A0` and `func_002008E8` (draw a part's strips)
  send every strip with the TEX0 read once before the loop, and inside the
  loop set CLD in each strip's own TEX0, which they never send.
  `func_00201518` also transforms the normals and loads the light matrix
  without using either. In all three, `if (packet_size < 3) return;` can never
  be true.
- `func_001A7B28` (homing effect) calls `func_001AFF78` (nearest entity:
  effect, position) with a third argument, the model's rotation. The callee
  does not read it.
- `func_001A9F98` keeps an effect's restart frame in a byte and sets
  `effect->frame = (s8)start_frame`; `func_001AA040` puts it back the same
  way, `(s8)motion->start_frame`, from a `u8`. `frame` is a `u16`, so a start
  frame of 128 or more would become a frame near 0xFF80. Not checked whether
  any definition's `unkF0 + interval` gets that high.
- `func_001AF640` (place a new effect's model) first copies the definition's
  `offset_base` (flag 0x10000) or the slot's position into the model's
  position. Nothing reads it before the `sceVu0AddVector` at the end writes
  the position again.
- `func_001D37F8` and `func_001D39F0` (rotate the upload nodes) pass their own
  first argument to `func_001D3730` and `func_001D3650`, which take none.
- `func_0010B758` (set up a model's parts) walks each part's data blocks by
  their lengths and uses nothing from the walk; retail keeps only the empty
  loop.
- Stack space that is reserved and never used: two vectors in `func_001AF0B0`
  and in `func_001A1E88`, a matrix in `func_001A6538` and `func_001AB670`,
  0x70 bytes in `func_001EEA90`, eight floats in `func_001AFF78`.
