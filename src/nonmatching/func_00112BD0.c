/* Map_UpdateDrawState (0x15C4 bytes, 1393 instructions): NOT MATCHED. This file is not built.
 *
 * The per-frame map draw-state update, called once per frame from the main loop: fades the fog block and the light
 * block towards those of the map record the player stands on (30 frames), culls the map records and builds the
 * draw lists.
 *
 * It lives outside src/game because carve.py compiles and places everything under src/game, and this does not match:
 * the ROM is built from the function's assembly. Compiled with the game's flags (-O2 -G 8) it gives 1393 instructions
 * of which 31 differ from retail, all in one basic block, the fog blend at 0x1131DC-0x1132E0 (search "fog blend"
 * below). Code outside that block, the jump table and the three float literals (1.3f, 0.9604f, 0.3978f) equal retail.
 *
 * What differs: the block has the same 50 instructions as retail, in another order and with other FP register
 * numbers. Retail's second scheduling pass issues the four float loads (0x148, 0x168, 0x14C, 0x16C off the draw state)
 * one per cycle AFTER all six colour bytes are converted, and `addiu $2,$9,0xB0` one instruction before
 * `sb $3,1($2)`; every source form tried lets the loads fill the idle slots earlier and puts the addiu about nine
 * instructions early. So the C below computes what retail computes; it is the order that is not reproduced.
 *
 * What is known (five agent passes, two search tools, a simulator of both scheduling passes checked against this
 * compiler's own source, github.com/SSXModding/ps2-ee-toolchain ee/gcc):
 *  - Under the compiler's rules retail's order is unreachable from these 50 instructions by any input order: retail's
 *    RTL for the block must have carried a dependence ours lacks (the loads wait for the last "to" byte conversion,
 *    the addiu for the two near-distance multiplies), with no register or memory in common.
 *  - Ruled out: compiler flags (about 60 sets); a different scheduler or r5900 unit model; function-unit conflicts;
 *    a hidden scratch register of fix_truncsfsi2; the conversion as two instructions (cvt.w.s into an FP register plus
 *    a separate mfc1: this compiler cannot keep an int in an FP register, and the pair would not stay adjacent);
 *    passes after the second scheduling pass; no-op moves and clobbers deleted late; links carried over from the first
 *    pass; a silent divide trap (the function has no integer division).
 *  - Source forms tried in the block without changing the schedule: every order of the five statements, operands
 *    swapped, float and int temporaries shared in about 9,000 generated combinations, pointer and struct views of the
 *    colours, volatile and const accesses, local copies of the fog block, do { } while (0) wrappers.
 *  - The form kept here is the lowest count found (31). It came out of randomised searches over temporaries (34),
 *    then decomp-permuter (the operand order of the last blend, 33; the dead store into blue_to, 31), so its exact
 *    shape is not evidence of the original source; a plainer version with five statements differs in 41.
 *  - Pinning registers (`register f32 blue_from asm("$f5"); register f32 blue_to asm("$f0");`) gives 29: every
 *    FP register in the block then equals retail, and only the order differs. Not kept, as it still does not match.
 *    No other subset of the block's temporaries pinned to retail's registers, including the four float loads, does
 *    better.
 *  - An empty `__asm__ volatile("")` (with or without a "memory" clobber), alone or in pairs, at every statement
 *    boundary of the block makes it worse (44 or more). With the statements in retail's split order (the six
 *    byte-to-float conversions, the red sum and the green multiplies first) one barrier does put the four float loads
 *    after all six conversions, as in retail, but the rest moves: 53, and 40 at best with registers pinned.
 *  - The compiler's own scheduler log (-fsched-verbose-5, haifa-sched) agrees with retail cycle for cycle up to
 *    cycle 10; at cycle 11 it issues the first float load, ready since cycle 1, where retail waits for the last two
 *    byte conversions. Retail behaves as if the second pass saw a barrier there.
 *  - A do { } while (0) around the second half (loop notes are a haifa-sched barrier) reproduces the load timing
 *    but pads the next label (the else branch) to 8 bytes with a nop that retail lacks; with the nop removed by
 *    hand the rest of the function matches and the block differs in 57. Unused labels are deleted (no effect);
 *    pinned ones (&&label) split the block for CSE too. A whole-block static inline helper gives 60 or more, and
 *    ee-gcc 2.96 already differs at the start of the function.
 *
 * Sixth pass (the compiler source read for every way an insn can come to depend on what precedes it; no source form
 * found, the C below is unchanged apart from today's names):
 *  - The second pass issues two instructions a cycle (ISSUE_RATE is 2 for the r5900): one floating-point operation
 *    and one filler. Counted so, retail's four float loads become ready at cycle 17, the cycle after the "to" blue
 *    byte is converted, and `addiu $2,$9,0xB0` at cycle 25, the cycle after the second near-distance multiply; all
 *    else is issued as here. Put another way, retail reads as three runs of 25, 13 and 12 instructions, each scheduled
 *    like a block of its own (the loads fill the second run's first cycles, the addiu the third's first cycle).
 *  - haifa-sched.c never clears LOG_LINKS: the first pass inherits the links of flow and combine, the second pass
 *    every link of the first whose two insns are still in one block (the dumps show them piling up). Register links
 *    carried over are ones the second pass finds anyway; memory, barrier and stale links are not. (The seventh
 *    pass shows the missing dependences cannot have been in the first pass in this form.)
 *  - Ruled out from the source: the pending-list flush (above 32 references; the flushing insn is a store and retail
 *    issues all ten loads before the five stores); loop, EH and setjmp notes (the insn after the note depends on every
 *    earlier setter at full cost, and the block would split in source order, which retail's split is not); the unused
 *    GPR scratch of fix_truncsfsi2 (an output dependence, cost 1); no-op moves (reload_cse_regs deletes them before
 *    the second pass); an unused label or a jump to the next insn inside the block (find_basic_blocks merges such
 *    blocks before either pass; two identical arms that jump2 would cross-jump away were not tried); interblock
 *    motion (the switch tables make the CFG "nonregular": both passes schedule single blocks);
 *    update_equiv_regs in local-alloc (it re-emits only single-use constants defined in another block); the
 *    LOOP_ALIGN of a do { } while (0) lands on the next label wherever it is, here the unaligned else label.
 *  - Tried without moving the loads: bit-field views of the colour (64-bit fields add dsll32/dsra32, 32-bit ones
 *    compile like u8 members), u8 pointer variables and indexed access (cse resolves them; `*p++` changes the code),
 *    u32 temporaries (the unsigned fix-up branch stays), the int or the two fade variables shared with the other
 *    blends, the float statements before or after the colour statements, (u8) casts. The plain form with the two
 *    float statements first and one int temporary differs in 41.
 *
 * Seventh pass (the compiler rebuilt from source with a hook that dumps and injects scheduler dependences; the rebuilt
 * cc1 gives the stock compiler's bytes and RTL dumps for this file once four templates of the -01 release, the
 * "nop; nop" before div.s, sqrt.s and rsqrt.s, are put into the 991111 tree):
 *  - The exact target. Take the C below with the two register pins, write the near blend plainly
 *    (`unkA0.near = unk140.near * fade_out + unk160.near * fade_in;`) and after it the far blend through two float
 *    variables with the "to" value loaded first; every register then equals retail. Add, in the SECOND pass only,
 *    "each of the four float loads depends on the conversion of the to-blue byte" and "the addiu depends on both
 *    near-distance multiplies" (anti or output, cost 1): 2 instructions differ, the two far loads in the other order,
 *    a tie; with one more link for that tie the whole function is byte-identical to retail (1393 of 1393).
 *  - The same links given to the FIRST pass (the second inherits them) break the register allocation (39 differ):
 *    retail's red conversion uses scratch $f11, so when registers were assigned all four float loads already
 *    preceded it; the loads were held back in the second pass but not in the first.
 *  - Nothing in the second pass's own analysis of these 50 insns, nor in the links flow adds after reload, can
 *    relate a float load to that conversion or the addiu to the multiplies (no register, no memory, and this
 *    compiler has no rule that treats REG_DEAD notes as uses). The two source insns are simply the last ones issued
 *    before each held group, so the second pass must have seen the block cut in three at those points of the
 *    first pass's order, or two insns that are no longer there. Where that comes from is still open.
 *  - Also tried: an if/else whose two arms are identical is cross-jumped away by jump2 and leaves no trace, and its
 *    arms are scheduled as blocks of their own (so such a cut is possible), but a cut made in the source falls on a
 *    statement boundary, which retail's cuts do not; a statement macro per field as do { } while (0) (loop
 *    alignment, 500 and more differ); inline helpers per field or for the whole blend (73 to 75); a loop over the
 *    three bytes (other code, 1379 instructions); a local copy of the timer (changes the code before the block). About 11,500
 *    plain forms of the five statements scored with the links injected: none reproduces retail's registers.
 *
 * Eighth pass (same hooked compiler; nothing below changes the target of the seventh pass):
 *  - Reproducing it: the hooked compiler is /home/sean/eegcc-kf on the WSL machine it was built on (cc1-kf, kfcc.sh,
 *    rebuild-kf.sh; KF_FUNC selects the function, KF_DUMP lists every LOG_LINK with kind and origin, KF_INJECT adds
 *    links, KF_STRIP drops inherited links before the second pass, KF_SPLIT_B/C/PASS really split a block). The
 *    scripts and the candidate source (ok_candidate.c) are in that session's scratch folder kf4, with NOTES.md.
 *    `python3 inj.py bc14 'L<-FLBT:a:2,PD<-MNF:a:2,PD<-MNT:a:2,LFT<-LFF:a:2' 12` prints OK (1393
 *    instructions):
 *    L = the four float loads, FLBT = the conversion of the to-blue byte, PD = the addiu, MNF/MNT = the two near
 *    multiplies, LFT<-LFF the tie between the far loads; a = anti, 2 = second pass.
 *  - Really scheduling the block as three blocks in the second pass (cut before the first float load and before
 *    the addiu) does not reproduce retail (10 differ: block-local priorities swap the two address addius and move
 *    the red conversion). Retail scheduled one block; its three contiguous runs are what the links produce.
 *  - A struct constructor (`x = (T){ a, b, c }`) does emit a (clobber (mem)) that acts as a store in the first pass,
 *    but reload deletes every such clobber, so it is gone before the second pass; the compound-literal forms tried
 *    also change the code (stack temporary or a materialised address).
 *  - The second pass takes its blocks from a fresh find_basic_blocks after reload, so stale block tables are not
 *    possible; RANGE notes are not scheduling barriers; no pattern this block uses has a trap, an unspec_volatile
 *    or a scratch other than the two of fix_truncsfsi2; the pending lists are reset for each block.
 *  - Stores written as an if/else with identical arms inside the colour statements are not merged (code grows).
 *  - Stripping all inherited links before the second pass leaves this block's links unchanged: with today's C
 *    nothing is inherited here that the second pass does not find itself.
 *
 * The struct views below are local because the file is not part of the build; when the function matches they belong
 * in map/map.h (MapLightBlockFloats = MapLightBlock as two 4x4 float matrices, 8-aligned so that copies use ld/sd;
 * MapDrawStateBlend = map_draw_state as this function sees it; MapHeader gains fog_colors[4][4] at 0x50 and the f32 draw
 * distance at 0x7C; MapRecordBits gains bit 26, "in view").
 */
#define MAP_DRAW_STATE_DEFINITION /* map_draw_state is declared below as this function sees it */
#include "map/map.h"
#include "map/map_scroll.h"
#include "entity/entity.h"
#include "enemy/enemy.h"
#include "player/player.h"
#include "core/game.h"
#include "core/file.h"
#include "core/memory.h"
#include "collision/collision.h"
#include "math/camera.h"
#include "math/vu0.h"
#include "model/model_matrix.h"
#include "libvu0.h"
#include "math.h"

typedef struct {
    f32 unk00[4][4];
    f32 unk40[4][4];
} __attribute__((aligned(8))) MapLightBlockFloats;

typedef struct {
    MapLightBlockFloats blocks[0x20];
} MapLightRow;

typedef struct {
    char pad00[0x50];
    u8 fog_colors[4][4];
    char pad60[0x1C];
    f32 draw_distance;
} MapHeaderFog;

typedef struct {
    char pad00[0x38];
    u64 pad38_0 : 26;
    u64 in_view : 1;
    u64 map : 1;
    u64 pad38_28 : 36;
} MapRecordBits2;
#define BITS2(record) ((MapRecordBits2 *)(record))

typedef struct {
    char pad00[0x60];
    f32 mirror_height;
    u8 mode;
    char pad65[0x35];
    u8 unk9A;
    u8 unk9B;
    u8 unk9C;
    char pad9D[0x03];
    MapFogBlockValues unkA0;
    MapLightBlockFloats unkC0;
    MapFogBlockValues unk140;
    MapFogBlockValues unk160;
    MapLightBlockFloats unk180;
    MapLightBlockFloats unk200;
    u8 fog[4];
    u8 fog_from[4];
    u8 fog_to[4];
    char pad28C[0x04];
    MapFogBlockValues unk290[2][0x20];
    MapLightRow unkA90[2];
    f32 unk2A90;
    f32 unk2A94;
    f32 unk2A98;
    char pad2A9C[0x04];
} MapDrawStateBlend;

extern MapDrawStateBlend map_draw_state;
void Wobble_ApplyWaveUvs(ModelInstance *);

#define BOUNDS(record) ((MapRecordBounds *)(record))
#define RECORD_FLAGS(record) (((MapRecord *)(record))->record_flags)

#define CLAMP_UNIT(value) \
    if ((value) < 0.0f) { \
        (value) = 0.0f; \
    } else if ((value) > 1.0f) { \
        (value) = 1.0f; \
    }

#define ROW_DIFFERS(a, b, row) \
    ((a).unk00[row][0] != (b)->unk00[row][0] || (a).unk00[row][1] != (b)->unk00[row][1] || \
     (a).unk00[row][2] != (b)->unk00[row][2] || (a).unk00[row][3] != (b)->unk00[row][3])
#define ROW2_DIFFERS(a, b, row) \
    ((a).unk40[row][0] != (b)->unk40[row][0] || (a).unk40[row][1] != (b)->unk40[row][1] || \
     (a).unk40[row][2] != (b)->unk40[row][2] || (a).unk40[row][3] != (b)->unk40[row][3])
#define LIGHTS_DIFFER(a, b) \
    (ROW_DIFFERS(a, b, 0) || ROW_DIFFERS(a, b, 1) || ROW_DIFFERS(a, b, 2) || ROW_DIFFERS(a, b, 3) || \
     ROW2_DIFFERS(a, b, 0) || ROW2_DIFFERS(a, b, 1) || ROW2_DIFFERS(a, b, 2) || ROW2_DIFFERS(a, b, 3))

#define FOG_DIFFERS(a, b) \
    ((a).fog_base_value != (b)->fog_base_value || (a).fog_inverse_depth_scale != (b)->fog_inverse_depth_scale || \
     (a).fog_colour != (b)->fog_colour)

#define CORNER_DISTANCE(x, z) \
    delta_x = (x) - camera_state.position[0]; \
    delta_z = (z) - camera_state.position[2]; \
    delta_x = ABS(delta_x); \
    delta_z = ABS(delta_z); \
    if (delta_x > delta_z) { \
        distance = delta_x * 0.9604f + delta_z * 0.3978f; \
    } else { \
        distance = delta_z * 0.9604f + delta_x * 0.3978f; \
    }

#define UPDATE_MODEL_MATRIX(record)     sceVu0CopyMatrix((record)->model->model_matrix, identity_matrix);     MODEL_MATRIX((record)->model)[0][0] *= (record)->model->scale_x;     MODEL_MATRIX((record)->model)[1][1] *= (record)->model->scale_y;     MODEL_MATRIX((record)->model)[2][2] *= (record)->model->scale_z;     sceVu0RotMatrixX((f32 *)(record)->model->model_matrix, (f32 *)(record)->model->model_matrix, (record)->model->rotation.x);     sceVu0RotMatrixY((f32 *)(record)->model->model_matrix, (f32 *)(record)->model->model_matrix, (record)->model->rotation.y);     sceVu0RotMatrixZ((f32 *)(record)->model->model_matrix, (f32 *)(record)->model->model_matrix, (record)->model->rotation.z);     sceVu0TransMatrix((f32 *)(record)->model->model_matrix, (f32 *)(record)->model->model_matrix, &(record)->model->position.x);     Matrix_Multiply((record)->model, world_screen_matrix, (record)->model->model_matrix);     for (k = 0; k < (record)->model->part_count; k++) {         ModelMatrix_BuildPartLocal((record)->model, &(record)->model->parts[k], &(record)->model->parts[k]);     }     sceVu0CopyMatrix((record)->model->collision_matrix, (record)->model->model_matrix)

typedef struct {
    u8 red;
    u8 green;
    u8 blue;
} MapFogRgb;
#define FOG_COLOR(fog) ((MapFogRgb *)&(fog)->fog_colour)

void Map_UpdateDrawState(void) {
    f32 unused_vector[4];
    f32 unused_matrix[32];
    MapSlot *slot;
    MapDrawRecord *record;
    MapLightBlockFloats *lights;
    MapFogBlockValues *fog_block;
    u8 *fog;
    f32 cos_heading;
    f32 sin_heading;
    f32 player_x;
    f32 player_z;
    f32 fade_out;
    f32 fade_in;
    f32 color_out;
    f32 color_in;
    f32 light_out;
    f32 light_in;
    f32 delta_x;
    f32 delta_z;
    f32 distance;
    f32 side_x0;
    f32 side_x1;
    f32 side_z0;
    f32 side_z1;
    int i;
    int j;
    int k;
    int value;
    int blended;

    memset(unused_vector, 0, sizeof(unused_vector));
    unused_vector[2] = -1.0f;
    MapScroll_PrepareCopy();
    MapScroll_PrepareDraw();

    slot = &map_slots[current_map_slot];
    fog = ((MapHeaderFog *)slot->header)->fog_colors[(u8)(RECORD_FLAGS(slot->records + player_state.map_index * 0x80) >> 24) & 3];
    if ((map_draw_state.unk9C == 0 && (fog[0] != map_draw_state.fog[0] || fog[1] != map_draw_state.fog[1] || fog[2] != map_draw_state.fog[2])) ||
        (map_draw_state.unk9C != 0 && (fog[0] != map_draw_state.fog_to[0] || fog[1] != map_draw_state.fog_to[1] || fog[2] != map_draw_state.fog_to[2]))) {
        map_draw_state.fog_from[0] = map_draw_state.fog[0];
        map_draw_state.fog_from[1] = map_draw_state.fog[1];
        map_draw_state.fog_from[2] = map_draw_state.fog[2];
        map_draw_state.fog_to[0] = fog[0];
        map_draw_state.fog_to[1] = fog[1];
        map_draw_state.fog_to[2] = fog[2];
        map_draw_state.unk9C = 30;
    }
    if (map_draw_state.unk9C != 0) {
        map_draw_state.unk9C--;
        if (map_draw_state.unk9C != 0) {
            color_out = map_draw_state.unk9C;
            color_in = 30 - map_draw_state.unk9C;
            color_out /= 30.0f;
            color_in /= 30.0f;
            for (i = 0; i < 3; i++) {
                value = (int)(map_draw_state.fog_from[i] * color_out + map_draw_state.fog_to[i] * color_in);
                map_draw_state.fog[i] = value > 0xFF ? 0xFF : value;
            }
        }
    }

    if (map_slots[0].loaded != 0) {
        Memory_CopyWords((s32 *)map_draw_state.unk290[0], (s32 *)map_slots[0].fog_blocks, 0x100);
    }
    if (map_slots[1].loaded != 0) {
        Memory_CopyWords((s32 *)map_draw_state.unk290[1], (s32 *)map_slots[1].fog_blocks, 0x100);
    }
    for (i = 0, slot = map_slots; i < 2; i++, slot++) {
        if (slot->loaded != 0) {
            lights = map_draw_state.unkA90[i].blocks;
            *(MapLightRow *)lights = *(MapLightRow *)slot->light_blocks;
            for (j = 0; j < 0x20; j++, lights++) {
                switch (slot->tag) {
                case 0:
                case 2:
                case 4:
                case 13:
                case 21:
                case 27:
                case 30:
                case 42:
                case 98:
                case 99:
                    break;
                default:
                    lights->unk40[3][0] *= 1.3f;
                    lights->unk40[3][1] *= 1.3f;
                    lights->unk40[3][2] *= 1.3f;
                    break;
                }
                switch (slot->tag) {
                case 0:
                case 2:
                case 4:
                case 13:
                case 30:
                case 42:
                case 98:
                case 99:
                    break;
                default:
                    lights->unk40[3][0] += map_draw_state.unk2A90;
                    lights->unk40[3][1] += map_draw_state.unk2A94;
                    lights->unk40[3][2] += map_draw_state.unk2A98;
                    break;
                }
                CLAMP_UNIT(lights->unk40[3][0]);
                CLAMP_UNIT(lights->unk40[3][1]);
                CLAMP_UNIT(lights->unk40[3][2]);
            }
        }
    }

    slot = &map_slots[current_map_slot];
    record = (MapDrawRecord *)slot->records + player_state.map_index;
    fog_block = &map_draw_state.unk290[current_map_slot][record->fog_block_index];
    if ((map_draw_state.unk9A == 0 && FOG_DIFFERS(map_draw_state.unkA0, fog_block)) || (map_draw_state.unk9A != 0 && FOG_DIFFERS(map_draw_state.unk160, fog_block))) {
        map_draw_state.unk9A = 30;
        map_draw_state.unk140 = map_draw_state.unkA0;
        map_draw_state.unk160 = *fog_block;
    }
    if (map_draw_state.unk9A != 0) {
        map_draw_state.unk9A--;
        if (map_draw_state.unk9A != 0) {
            { /* fog blend: the 31 instructions that differ come from here */
                f32 blend_sum;
                f32 green_from;
                f32 green_to;
                f32 blue_from;
                f32 blue_to;
                f32 far_to;
                /* match: reuse the red blend temporary for the near-distance blend. */
                fade_out = map_draw_state.unk9A;
                fade_in = 30 - map_draw_state.unk9A;
                fade_out /= 30.0f;
                fade_in /= 30.0f;
                blend_sum = FOG_COLOR(&map_draw_state.unk140)->red * fade_out + FOG_COLOR(&map_draw_state.unk160)->red * fade_in;
                blended = (int)(blend_sum);
                FOG_COLOR(&map_draw_state.unkA0)->red = blended;
                green_from = FOG_COLOR(&map_draw_state.unk140)->green;
                green_to = FOG_COLOR(&map_draw_state.unk160)->green;
                green_from *= fade_out;
                green_to *= fade_in;
                green_from += green_to;
                blended = (int)(green_from);
                FOG_COLOR(&map_draw_state.unkA0)->green = blended;
                blue_from = FOG_COLOR(&map_draw_state.unk140)->blue;
                blue_to = FOG_COLOR(&map_draw_state.unk160)->blue;
                blue_from *= fade_out;
                blue_to *= fade_in;
                blue_from += blue_to;
                blended = (int)(blue_from);
                FOG_COLOR(&map_draw_state.unkA0)->blue = blended;
                far_to = map_draw_state.unk160.fog_inverse_depth_scale;
                far_to *= fade_in;
                map_draw_state.unkA0.fog_inverse_depth_scale = map_draw_state.unk140.fog_inverse_depth_scale * fade_out + far_to;
                /* match: the dead store into blue_to (decomp-permuter) takes this from 33 to 31. */
                blend_sum = (blue_to = map_draw_state.unk160.fog_base_value * fade_in + map_draw_state.unk140.fog_base_value * fade_out);
                map_draw_state.unkA0.fog_base_value = blend_sum;
            }
        } else {
            map_draw_state.unkA0 = map_draw_state.unk160;
        }
    }

    slot = &map_slots[current_map_slot];
    record = (MapDrawRecord *)slot->records + player_state.map_index;
    lights = &map_draw_state.unkA90[current_map_slot].blocks[record->light_block_index];
    if ((map_draw_state.unk9B == 0 && LIGHTS_DIFFER(map_draw_state.unkC0, lights)) || (map_draw_state.unk9B != 0 && LIGHTS_DIFFER(map_draw_state.unk200, lights))) {
        map_draw_state.unk9B = 30;
        map_draw_state.unk180 = map_draw_state.unkC0;
        map_draw_state.unk200 = *lights;
    }
    if (map_draw_state.unk9B != 0) {
        map_draw_state.unk9B--;
        if (map_draw_state.unk9B != 0) {
            light_out = map_draw_state.unk9B;
            light_in = 30 - map_draw_state.unk9B;
            light_out /= 30.0f;
            light_in /= 30.0f;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    map_draw_state.unkC0.unk00[i][j] = map_draw_state.unk180.unk00[i][j] * light_out + map_draw_state.unk200.unk00[i][j] * light_in;
                    map_draw_state.unkC0.unk40[i][j] = map_draw_state.unk180.unk40[i][j] * light_out + map_draw_state.unk200.unk40[i][j] * light_in;
                }
            }
        } else {
            map_draw_state.unkC0 = map_draw_state.unk200;
        }
    }

    map_draw_state.mode = 0;
    record_draw_lists[0] = record_draw_lists[1] = NULL;
    late_record_draw_lists[0] = late_record_draw_lists[1] = NULL;
    cos_heading = cosf(player_state.rotation.y);
    sin_heading = sinf(player_state.rotation.y);
    player_x = player_state.position.x;
    player_z = player_state.position.z;
    for (i = 0; i < 2; i++) {
        slot = &map_slots[i];
        if (slot->loaded != 0) {
            record = (MapDrawRecord *)slot->records + (slot->header->record_count - 1);
            for (j = slot->header->record_count - 1; j >= 0; j--, record--) {
                if (record->flags & 0x10000000) {
                    UPDATE_MODEL_MATRIX(record);
                }
                BITS2(record)->in_view = FALSE;
                if (record->flags & 0x40000000) {
                    continue;
                }
                if (BOUNDS(record)->model_index == 0xFFFF) {
                    continue;
                }
                if (((MapHeaderFog *)slot->header)->draw_distance > 0.0f) {
                    CORNER_DISTANCE(BOUNDS(record)->box_lowest[0], BOUNDS(record)->box_lowest[2]);
                    if (distance > ((MapHeaderFog *)slot->header)->draw_distance) {
                        CORNER_DISTANCE(BOUNDS(record)->box_lowest[0], BOUNDS(record)->box_highest[2]);
                        if (distance > ((MapHeaderFog *)slot->header)->draw_distance) {
                            CORNER_DISTANCE(BOUNDS(record)->box_highest[0], BOUNDS(record)->box_lowest[2]);
                            if (distance > ((MapHeaderFog *)slot->header)->draw_distance) {
                                CORNER_DISTANCE(BOUNDS(record)->box_highest[0], BOUNDS(record)->box_highest[2]);
                                if (distance > ((MapHeaderFog *)slot->header)->draw_distance) {
                                    continue;
                                }
                            }
                        }
                    }
                }
                side_x0 = (BOUNDS(record)->box_lowest[0] - player_x) * sin_heading;
                side_z0 = (BOUNDS(record)->box_lowest[2] - player_z) * cos_heading;
                if (!(side_x0 + side_z0 >= 0.0f)) {
                    side_x1 = (BOUNDS(record)->box_highest[0] - player_x) * sin_heading;
                    if (!(side_x1 + side_z0 >= 0.0f)) {
                        side_z1 = (BOUNDS(record)->box_highest[2] - player_z) * cos_heading;
                        if (!(side_x1 + side_z1 >= 0.0f)) {
                            if (!(side_x0 + side_z1 >= 0.0f)) {
                                continue;
                            }
                        }
                    }
                }
                if (Map_TestFlagBlockBit(current_map_slot, player_state.record_flag_block, i, BOUNDS(record)->flag_block_bit) == 0) {
                    continue;
                }
                BITS2(record)->in_view = TRUE;
                if (map_draw_state.mode == 0) {
                    if (record->flags & 2) {
                        map_draw_state.mode = 1;
                        map_draw_state.mirror_height = BOUNDS(record)->position[1];
                    } else if (record->flags & 0x200) {
                        map_draw_state.mode = 2;
                        map_draw_state.mirror_height = BOUNDS(record)->position[1];
                    }
                }
                if (!(record->flags & 1)) {
                    record->next = record_draw_lists[i];
                    record_draw_lists[i] = record;
                } else {
                    record->next = late_record_draw_lists[i];
                    late_record_draw_lists[i] = record;
                }
                if (record->flags < 0) {
                    Vector_Copy(&record->model->position, &player_state);
                }
                if (!(record->flags & 0x10000000)) {
                    UPDATE_MODEL_MATRIX(record);
                }
                if (record->flags & 0x100) {
                    Wobble_ApplyWaveUvs(record->model);
                }
            }
        }
    }
}
