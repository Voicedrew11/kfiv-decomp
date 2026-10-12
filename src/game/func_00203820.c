#include "kf_game.h"
#include <libvu0.h>

#define ANGLE_PI 3.1415927f
#define ANGLE_TURN 6.283184f
/* an angle brought into -PI..PI */
#define WRAP_ANGLE(angle) ((angle) + ANGLE_PI - (f32)(s32)(((angle) + ANGLE_PI) / ANGLE_TURN) * ANGLE_TURN - ANGLE_PI)

void *memset(void *, int, u32);
void sceVu0TransMatrix(float *, float *, float *);
// match: the phase last, so that it is set up after the pointers (the definition has it first; same registers)
void func_002037A8(void *, void *, void *, void *, void *, float);

/* Poses a node of a skeleton at a frame of its animation, into its slot's matrix, then its children and siblings. */
void func_00203820(RelocEntry *node, PoolSlot *slots, f32 *parent_matrix, s32 frame) {
    f32 local[4][4];
    f32 unused[4][4]; /* unused, but retail reserves the 0x40 bytes */
    f32 unit[4][4];
    f32 rotation[4];
    f32 translation[4];
    f32 scale[4];
    f32 key_scale[4];
    f32 next_scale[4];
    PoolSlot *slot;
    PoolNodeKey *key;
    PoolNodeKey *next;
    f32 phase;

    memset(unit, 0, 0x40);
    unit[3][3] = unit[2][2] = unit[1][1] = unit[0][0] = 1.0f;
    slot = &slots[node->slot]; // match: node is read here, before the test for NULL below
    if (node != NULL) {
        if (frame == 0) {
            slot->key_index = 0;
        }
        // match: with no keys, key, next and phase are used below without being set
        if (node->unk30 != NULL) {
            key = &((PoolNodeKey *)node->unk30)[slot->key_index];
            for (;;) {
                if (frame < key->frame + key->length) {
                    phase = (f32)(frame - key->frame) / (f32)key->length;
                    next = key + 1;
                    break;
                }
                next = key;
                if (node->key_count < (u32)(slot->key_index + 1)) {
                    phase = 0.0f;
                    break;
                }
                slot->key_index++;
                key++;
            }
        }
        func_002037A8(rotation, key->rotation, next->rotation, key->rotation_out, next->rotation_in, phase);
        sceVu0RotMatrixX(local, unit, WRAP_ANGLE(rotation[0]));
        sceVu0RotMatrixZ(local, local, WRAP_ANGLE(rotation[2]));
        sceVu0RotMatrixY(local, local, WRAP_ANGLE(rotation[1]));
        func_002037A8(translation, key->translation, next->translation, key->translation_out, next->translation_in, phase);
        sceVu0TransMatrix(local[0], local[0], translation);
        sceVu0ScaleVectorXYZ(key_scale, key->scale, 1.0f - phase);
        sceVu0ScaleVectorXYZ(next_scale, next->scale, phase);
        sceVu0AddVector(scale, key_scale, next_scale);
        sceVu0ScaleVectorXYZ(local[0], local[0], scale[0]);
        sceVu0ScaleVectorXYZ(local[1], local[1], scale[1]);
        sceVu0ScaleVectorXYZ(local[2], local[2], scale[2]);
    } else {
        sceVu0RotMatrixX(local, unit, WRAP_ANGLE(slot->part->rotation[0]));
        sceVu0RotMatrixY(local, local, WRAP_ANGLE(slot->part->rotation[1]));
        sceVu0RotMatrixZ(local, local, WRAP_ANGLE(slot->part->rotation[2]));
        sceVu0TransMatrix(local[0], local[0], slot->part->position);
        sceVu0ScaleVectorXYZ(local[0], local[0], slot->part->scale[0]);
        sceVu0ScaleVectorXYZ(local[1], local[1], slot->part->scale[1]);
        sceVu0ScaleVectorXYZ(local[2], local[2], slot->part->scale[2]);
    }
    sceVu0MulMatrix(slot, parent_matrix, local);
    if (node->unk38 != NULL) {
        func_00203820(node->unk38, slots, (f32 *)slot, frame);
    }
    if (node->unk3C != NULL) {
        func_00203820(node->unk3C, slots, parent_matrix, frame);
    }
}
