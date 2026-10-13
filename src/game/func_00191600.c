#include "kf_entity.h"
#include "libvu0.h"

s32 func_00191A60(ModelInstance *, s32, s32, s32, s32, s32);
void func_00216778(void *, void *);
void sceVu0MulVector(void *, void *, void *);
void Vector_Subtract(void *, void *, void *);
void Vector_Scale(float *, float *, float);
float sinf(float);
float cosf(float);

extern f32 D_00431AF0[16];

/* Plays one of the entity's two motion slots a step on and, for a slot that moves the entity, turns what the
   root part moved by into the entity's own movement; returns whether the motion ended. */
s32 func_00191600(Entity *entity, EntityUnkE0SlotBits *slot_bits, s32 type, s32 mode) {
    EntityMotionSlot *slot = (EntityMotionSlot *)slot_bits;
    f32 moved[4];
    f32 turned[4];
    EntityParams *params;
    MotionData *motion;
    ModelPart *root;
    s32 table;
    s32 step;
    f32 sine;
    f32 cosine;

    params = entity->params;
    table = params->unk60[slot->index];
    // match: indexing level_tables as an array swaps the addu operands
    motion = *(MotionData **)((char *)ENTITY_MODEL(entity)->level_tables + (table << 2));
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    if (entity->unk28 != 0 && (entity->unkC6 & 0x46) == 0) {
        step = (f32)params->unk80[entity->unkD1] * 0.75f;
    } else {
        step = params->unk80[entity->unkD1];
    }
    /* a motion standing on its first frame is stepped once more than the others */
    // match: one if with both tests joined by a ternary lays the branches out differently
    if (slot->backwards) {
        if (slot->frame == motion->frame_count) {
            slot->frame_fixed = func_00191A60(ENTITY_MODEL(entity), type, table, slot->frame_fixed, step, slot->backwards);
        }
    } else if (slot->frame == 0) {
        slot->frame_fixed = func_00191A60(ENTITY_MODEL(entity), type, table, slot->frame_fixed, step, slot->backwards);
    }
    slot->frame = slot->frame_fixed >> 8;
    if (!slot->moves_entity) {
        slot->frame_fixed = func_00191A60(ENTITY_MODEL(entity), type, table, slot->frame_fixed, step, slot->backwards);
        slot->frame = slot->frame_fixed >> 8;
        return slot->backwards ? slot->frame == motion->frame_count : slot->frame == 0;
    }
    root = ENTITY_MODEL(entity)->unk130;
    turned[0] = 0.0f;
    turned[1] = 0.0f;
    turned[2] = 0.0f;
    turned[3] = 0.0f;
    moved[0] = 0.0f;
    moved[1] = 0.0f;
    moved[2] = 0.0f;
    moved[3] = 0.0f;
    func_00216778(turned, &root->unk50);
    func_00216778(moved, root->unk40);
    slot->frame_fixed = func_00191A60(ENTITY_MODEL(entity), type, table, slot->frame_fixed, step, slot->backwards);
    slot->frame = slot->frame_fixed >> 8;
    Vector_Subtract(turned, &root->unk50, turned);
    Vector_Subtract(moved, root->unk40, moved);
    sceVu0MulVector(moved, moved, &ENTITY_MODEL(entity)->unkE0);
    Vector_Scale(moved, moved, ENTITY_PARAMS_MOTION_SCALE(entity->params));
    if (ENTITY_MODEL(entity)->unkE0 < 0.0f) {
        turned[1] = -turned[1];
    }
    if (mode == 1) {
        sceVu0RotMatrixY((void *)0x70000000, D_00431AF0, -root->unk54);
        sceVu0RotMatrixX((void *)0x70000000, (void *)0x70000000, -root->unk50);
        sceVu0RotMatrixZ((void *)0x70000000, (void *)0x70000000, -root->unk58);
        sceVu0RotMatrixZ((void *)0x70000000, (void *)0x70000000, ENTITY_MODEL(entity)->rotation.z);
        sceVu0RotMatrixX((void *)0x70000000, (void *)0x70000000, ENTITY_MODEL(entity)->rotation.x);
        sceVu0RotMatrixY((void *)0x70000000, (void *)0x70000000, ENTITY_MODEL(entity)->rotation.y);
        sceVu0ApplyMatrix(moved, (void *)0x70000000, moved);
        entity->unk84 += moved[0];
        entity->unk88 += moved[1];
        entity->unk8C += moved[2];
        entity->unk90 += turned[0];
        entity->unk94 += turned[1];
        entity->unk98 += turned[2];
    } else if (mode == 2) {
        sine = sinf(-ENTITY_MODEL(entity)->rotation.y);
        cosine = cosf(-ENTITY_MODEL(entity)->rotation.y);
        entity->unk84 += moved[0] * cosine - moved[2] * sine;
        entity->unk8C += moved[0] * sine + moved[2] * cosine;
    } else if (mode == 3) {
        entity->unk94 += turned[1];
    }
    return slot->backwards ? slot->frame == motion->frame_count : slot->frame == 0;
}
