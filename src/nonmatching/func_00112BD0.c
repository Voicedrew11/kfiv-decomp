/* func_00112BD0 (0x15C4 bytes, 1393 instructions): NOT MATCHED. This file is not built.
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
 * The struct views below are local because the file is not part of the build; when the function matches they belong
 * in map/map.h (MapLightBlock = MapSlotUnk10Words as two 4x4 float matrices, 8-aligned so that copies use ld/sd;
 * MapDrawStateBlend = D_0038A1B0 as this function sees it; MapHeader gains fog_colors[4][4] at 0x50 and the f32 draw
 * distance at 0x7C; MapRecordBits gains bit 26, "in view").
 */
#include "map/map.h"
#include "entity/entity.h"
#include "player/player.h"
#include "core/game.h"
#include "collision/collision.h"

typedef struct {
    f32 unk00[4][4];
    f32 unk40[4][4];
} __attribute__((aligned(8))) MapLightBlock;

typedef struct {
    MapLightBlock blocks[0x20];
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
    MapSlotUnk0CValues unkA0;
    MapLightBlock unkC0;
    MapSlotUnk0CValues unk140;
    MapSlotUnk0CValues unk160;
    MapLightBlock unk180;
    MapLightBlock unk200;
    u8 fog[4];
    u8 fog_from[4];
    u8 fog_to[4];
    char pad28C[0x04];
    MapSlotUnk0CValues unk290[2][0x20];
    MapLightRow unkA90[2];
    f32 unk2A90;
    f32 unk2A94;
    f32 unk2A98;
    char pad2A9C[0x04];
} MapDrawStateBlend;

extern MapSlot D_0038CD20[];
extern MapDrawStateBlend D_0038A1B0;
extern PlayerState D_00413F80;
extern CameraState camera_state;
extern f32 identity_matrix[16];
extern f32 world_screen_matrix[16];
extern MapDrawRecord *record_draw_lists[2];
extern MapDrawRecord *late_record_draw_lists[2];
extern int current_map_slot;

void *memset(void *, int, u32);
float cosf(float);
float sinf(float);
void func_00116F90(void);
void func_001175F0(void);
void Memory_CopyWords(s32 *, s32 *, u32);
int func_00116C08(int, int, int, int);
void func_00112018(ModelInstance *);
void func_00216778(void *, void *);
void func_00216B48(void *, void *, void *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);
void sceVu0CopyMatrix(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0TransMatrix(float *, float *, float *);

#define BOUNDS(record) ((MapRecordBounds *)(record))
#define RECORD_FLAGS(record) (((MapRecord *)(record))->unk38)

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

#define FOG_DIFFERS(a, b) ((a).unk08 != (b)->unk08 || (a).unk0C != (b)->unk0C || (a).unk10 != (b)->unk10)

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

#define UPDATE_MODEL_MATRIX(record)     sceVu0CopyMatrix((record)->model->unk40, identity_matrix);     MODEL_MATRIX((record)->model)[0][0] *= (record)->model->unkE0;     MODEL_MATRIX((record)->model)[1][1] *= (record)->model->unkE4;     MODEL_MATRIX((record)->model)[2][2] *= (record)->model->unkE8;     sceVu0RotMatrixX((f32 *)(record)->model->unk40, (f32 *)(record)->model->unk40, (record)->model->rotation.x);     sceVu0RotMatrixY((f32 *)(record)->model->unk40, (f32 *)(record)->model->unk40, (record)->model->rotation.y);     sceVu0RotMatrixZ((f32 *)(record)->model->unk40, (f32 *)(record)->model->unk40, (record)->model->rotation.z);     sceVu0TransMatrix((f32 *)(record)->model->unk40, (f32 *)(record)->model->unk40, &(record)->model->position.x);     func_00216B48((record)->model, world_screen_matrix, (record)->model->unk40);     for (k = 0; k < (record)->model->part_count; k++) {         func_0020B8B0((record)->model, &(record)->model->parts[k], &(record)->model->parts[k]);     }     sceVu0CopyMatrix((record)->model->unk80, (record)->model->unk40)

typedef struct {
    u8 red;
    u8 green;
    u8 blue;
} MapFogRgb;
#define FOG_COLOR(fog) ((MapFogRgb *)&(fog)->unk10)

void func_00112BD0(void) {
    f32 unused_vector[4];
    f32 unused_matrix[32];
    MapSlot *slot;
    MapDrawRecord *record;
    MapLightBlock *lights;
    MapSlotUnk0CValues *fog_block;
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
    func_00116F90();
    func_001175F0();

    slot = &D_0038CD20[current_map_slot];
    fog = ((MapHeaderFog *)slot->header)->fog_colors[(u8)(RECORD_FLAGS(slot->records + D_00413F80.map_index * 0x80) >> 24) & 3];
    if ((D_0038A1B0.unk9C == 0 && (fog[0] != D_0038A1B0.fog[0] || fog[1] != D_0038A1B0.fog[1] || fog[2] != D_0038A1B0.fog[2])) ||
        (D_0038A1B0.unk9C != 0 && (fog[0] != D_0038A1B0.fog_to[0] || fog[1] != D_0038A1B0.fog_to[1] || fog[2] != D_0038A1B0.fog_to[2]))) {
        D_0038A1B0.fog_from[0] = D_0038A1B0.fog[0];
        D_0038A1B0.fog_from[1] = D_0038A1B0.fog[1];
        D_0038A1B0.fog_from[2] = D_0038A1B0.fog[2];
        D_0038A1B0.fog_to[0] = fog[0];
        D_0038A1B0.fog_to[1] = fog[1];
        D_0038A1B0.fog_to[2] = fog[2];
        D_0038A1B0.unk9C = 30;
    }
    if (D_0038A1B0.unk9C != 0) {
        D_0038A1B0.unk9C--;
        if (D_0038A1B0.unk9C != 0) {
            color_out = D_0038A1B0.unk9C;
            color_in = 30 - D_0038A1B0.unk9C;
            color_out /= 30.0f;
            color_in /= 30.0f;
            for (i = 0; i < 3; i++) {
                value = (int)(D_0038A1B0.fog_from[i] * color_out + D_0038A1B0.fog_to[i] * color_in);
                D_0038A1B0.fog[i] = value > 0xFF ? 0xFF : value;
            }
        }
    }

    if (D_0038CD20[0].unk00 != 0) {
        Memory_CopyWords((s32 *)D_0038A1B0.unk290[0], (s32 *)D_0038CD20[0].unk0C, 0x100);
    }
    if (D_0038CD20[1].unk00 != 0) {
        Memory_CopyWords((s32 *)D_0038A1B0.unk290[1], (s32 *)D_0038CD20[1].unk0C, 0x100);
    }
    for (i = 0, slot = D_0038CD20; i < 2; i++, slot++) {
        if (slot->unk00 != 0) {
            lights = D_0038A1B0.unkA90[i].blocks;
            *(MapLightRow *)lights = *(MapLightRow *)slot->unk10;
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
                    lights->unk40[3][0] += D_0038A1B0.unk2A90;
                    lights->unk40[3][1] += D_0038A1B0.unk2A94;
                    lights->unk40[3][2] += D_0038A1B0.unk2A98;
                    break;
                }
                CLAMP_UNIT(lights->unk40[3][0]);
                CLAMP_UNIT(lights->unk40[3][1]);
                CLAMP_UNIT(lights->unk40[3][2]);
            }
        }
    }

    slot = &D_0038CD20[current_map_slot];
    record = (MapDrawRecord *)slot->records + D_00413F80.map_index;
    fog_block = &D_0038A1B0.unk290[current_map_slot][record->unk34];
    if ((D_0038A1B0.unk9A == 0 && FOG_DIFFERS(D_0038A1B0.unkA0, fog_block)) || (D_0038A1B0.unk9A != 0 && FOG_DIFFERS(D_0038A1B0.unk160, fog_block))) {
        D_0038A1B0.unk9A = 30;
        D_0038A1B0.unk140 = D_0038A1B0.unkA0;
        D_0038A1B0.unk160 = *fog_block;
    }
    if (D_0038A1B0.unk9A != 0) {
        D_0038A1B0.unk9A--;
        if (D_0038A1B0.unk9A != 0) {
            { /* fog blend: the 31 instructions that differ come from here */
                f32 blend_sum;
                f32 green_from;
                f32 green_to;
                f32 blue_from;
                f32 blue_to;
                f32 far_to;
                /* match: reuse the red blend temporary for the near-distance blend. */
                fade_out = D_0038A1B0.unk9A;
                fade_in = 30 - D_0038A1B0.unk9A;
                fade_out /= 30.0f;
                fade_in /= 30.0f;
                blend_sum = FOG_COLOR(&D_0038A1B0.unk140)->red * fade_out + FOG_COLOR(&D_0038A1B0.unk160)->red * fade_in;
                blended = (int)(blend_sum);
                FOG_COLOR(&D_0038A1B0.unkA0)->red = blended;
                green_from = FOG_COLOR(&D_0038A1B0.unk140)->green;
                green_to = FOG_COLOR(&D_0038A1B0.unk160)->green;
                green_from *= fade_out;
                green_to *= fade_in;
                green_from += green_to;
                blended = (int)(green_from);
                FOG_COLOR(&D_0038A1B0.unkA0)->green = blended;
                blue_from = FOG_COLOR(&D_0038A1B0.unk140)->blue;
                blue_to = FOG_COLOR(&D_0038A1B0.unk160)->blue;
                blue_from *= fade_out;
                blue_to *= fade_in;
                blue_from += blue_to;
                blended = (int)(blue_from);
                FOG_COLOR(&D_0038A1B0.unkA0)->blue = blended;
                far_to = D_0038A1B0.unk160.unk0C;
                far_to *= fade_in;
                D_0038A1B0.unkA0.unk0C = D_0038A1B0.unk140.unk0C * fade_out + far_to;
                /* match: the dead store into blue_to (decomp-permuter) takes this from 33 to 31. */
                blend_sum = (blue_to = D_0038A1B0.unk160.unk08 * fade_in + D_0038A1B0.unk140.unk08 * fade_out);
                D_0038A1B0.unkA0.unk08 = blend_sum;
            }
        } else {
            D_0038A1B0.unkA0 = D_0038A1B0.unk160;
        }
    }

    slot = &D_0038CD20[current_map_slot];
    record = (MapDrawRecord *)slot->records + D_00413F80.map_index;
    lights = &D_0038A1B0.unkA90[current_map_slot].blocks[record->unk35];
    if ((D_0038A1B0.unk9B == 0 && LIGHTS_DIFFER(D_0038A1B0.unkC0, lights)) || (D_0038A1B0.unk9B != 0 && LIGHTS_DIFFER(D_0038A1B0.unk200, lights))) {
        D_0038A1B0.unk9B = 30;
        D_0038A1B0.unk180 = D_0038A1B0.unkC0;
        D_0038A1B0.unk200 = *lights;
    }
    if (D_0038A1B0.unk9B != 0) {
        D_0038A1B0.unk9B--;
        if (D_0038A1B0.unk9B != 0) {
            light_out = D_0038A1B0.unk9B;
            light_in = 30 - D_0038A1B0.unk9B;
            light_out /= 30.0f;
            light_in /= 30.0f;
            for (i = 0; i < 4; i++) {
                for (j = 0; j < 4; j++) {
                    D_0038A1B0.unkC0.unk00[i][j] = D_0038A1B0.unk180.unk00[i][j] * light_out + D_0038A1B0.unk200.unk00[i][j] * light_in;
                    D_0038A1B0.unkC0.unk40[i][j] = D_0038A1B0.unk180.unk40[i][j] * light_out + D_0038A1B0.unk200.unk40[i][j] * light_in;
                }
            }
        } else {
            D_0038A1B0.unkC0 = D_0038A1B0.unk200;
        }
    }

    D_0038A1B0.mode = 0;
    record_draw_lists[0] = record_draw_lists[1] = NULL;
    late_record_draw_lists[0] = late_record_draw_lists[1] = NULL;
    cos_heading = cosf(D_00413F80.rotation.y);
    sin_heading = sinf(D_00413F80.rotation.y);
    player_x = D_00413F80.position.x;
    player_z = D_00413F80.position.z;
    for (i = 0; i < 2; i++) {
        slot = &D_0038CD20[i];
        if (slot->unk00 != 0) {
            record = (MapDrawRecord *)slot->records + (slot->header->record_count - 1);
            for (j = slot->header->record_count - 1; j >= 0; j--, record--) {
                if (record->flags & 0x10000000) {
                    UPDATE_MODEL_MATRIX(record);
                }
                BITS2(record)->in_view = FALSE;
                if (record->flags & 0x40000000) {
                    continue;
                }
                if (BOUNDS(record)->unk30 == 0xFFFF) {
                    continue;
                }
                if (((MapHeaderFog *)slot->header)->draw_distance > 0.0f) {
                    CORNER_DISTANCE(BOUNDS(record)->unk40[0], BOUNDS(record)->unk40[2]);
                    if (distance > ((MapHeaderFog *)slot->header)->draw_distance) {
                        CORNER_DISTANCE(BOUNDS(record)->unk40[0], BOUNDS(record)->unk50[2]);
                        if (distance > ((MapHeaderFog *)slot->header)->draw_distance) {
                            CORNER_DISTANCE(BOUNDS(record)->unk50[0], BOUNDS(record)->unk40[2]);
                            if (distance > ((MapHeaderFog *)slot->header)->draw_distance) {
                                CORNER_DISTANCE(BOUNDS(record)->unk50[0], BOUNDS(record)->unk50[2]);
                                if (distance > ((MapHeaderFog *)slot->header)->draw_distance) {
                                    continue;
                                }
                            }
                        }
                    }
                }
                side_x0 = (BOUNDS(record)->unk40[0] - player_x) * sin_heading;
                side_z0 = (BOUNDS(record)->unk40[2] - player_z) * cos_heading;
                if (!(side_x0 + side_z0 >= 0.0f)) {
                    side_x1 = (BOUNDS(record)->unk50[0] - player_x) * sin_heading;
                    if (!(side_x1 + side_z0 >= 0.0f)) {
                        side_z1 = (BOUNDS(record)->unk50[2] - player_z) * cos_heading;
                        if (!(side_x1 + side_z1 >= 0.0f)) {
                            if (!(side_x0 + side_z1 >= 0.0f)) {
                                continue;
                            }
                        }
                    }
                }
                if (func_00116C08(current_map_slot, D_00413F80.unk250, i, BOUNDS(record)->unk38) == 0) {
                    continue;
                }
                BITS2(record)->in_view = TRUE;
                if (D_0038A1B0.mode == 0) {
                    if (record->flags & 2) {
                        D_0038A1B0.mode = 1;
                        D_0038A1B0.mirror_height = BOUNDS(record)->position[1];
                    } else if (record->flags & 0x200) {
                        D_0038A1B0.mode = 2;
                        D_0038A1B0.mirror_height = BOUNDS(record)->position[1];
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
                    func_00216778(&record->model->position, &D_00413F80);
                }
                if (!(record->flags & 0x10000000)) {
                    UPDATE_MODEL_MATRIX(record);
                }
                if (record->flags & 0x100) {
                    func_00112018(record->model);
                }
            }
        }
    }
}
