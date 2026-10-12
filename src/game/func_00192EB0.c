#include "kf_entity.h"
#include "libvu0.h"

s32 func_00193378(ModelInstance *, s32, s32);
void func_0018D670(Entity *, s32, s32, s32);
void func_0018D8B8(Entity *, s32, s32, s32);
void func_00216778(void *, void *);
void sceVu0MulVector(void *, void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void KF_Vector_Scale(float *, float *, float);
float sinf(float);
float cosf(float);

extern f32 D_00431AF0[16];

/* Plays the entity's motion a step on, turns what the root part moved by into the entity's own movement (mode 0:
   none of it) and starts what the motion sets off on the frames it passed; returns whether the motion ended. */
s32 func_00192EB0(Entity *entity, s32 mode) {
    f32 moved[4];
    f32 turned[4];
    ModelChannel *channel;
    MotionData *motion;
    ModelPart *root;
    s32 step;
    s32 frame_from;
    s32 frame_to;
    s32 swapped;
    f32 sine;
    f32 cosine;

    channel = MODEL_CHANNEL(ENTITY_MODEL(entity));
    motion = (MotionData *)channel->tables[channel->index];
    motion = (MotionData *)((ModelPartLevels *)motion - 1);
    if (entity->unk28 != 0 && (entity->unkC6 & 0x46) == 0) {
        step = (f32)entity->params->unk80[entity->unkD1] * 0.75f;
    } else {
        step = entity->params->unk80[entity->unkD1];
    }
    frame_from = channel->param_shifted;
    /* a motion standing on its first frame is stepped once more than the others */
    // match: one if with both tests joined by a ternary lays the branches out differently
    if (ENTITY_FLAG_BITS(entity)->bit15) {
        if (channel->param == motion->frame_count) {
            func_00193378(ENTITY_MODEL(entity), step, ENTITY_FLAG_BITS(entity)->bit15);
        }
    } else if (channel->param == 0) {
        func_00193378(ENTITY_MODEL(entity), step, ENTITY_FLAG_BITS(entity)->bit15);
    }
    if (mode == 0) {
        func_00193378(ENTITY_MODEL(entity), step, ENTITY_FLAG_BITS(entity)->bit15);
        frame_to = channel->param_shifted;
        swapped = frame_to;
        if (ENTITY_FLAG_BITS(entity)->bit15) {
            frame_to = frame_from;
            frame_from = swapped;
        }
        if (frame_from == frame_to) {
            frame_to = 0x100;
        }
        func_0018D670(entity, entity->unkD1, frame_from, frame_to);
        func_0018D8B8(entity, entity->unkD1, frame_from, frame_to);
        return ENTITY_FLAG_BITS(entity)->bit15 ? channel->param == motion->frame_count : channel->param == 0;
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
    func_00193378(ENTITY_MODEL(entity), step, ENTITY_FLAG_BITS(entity)->bit15);
    KF_Vector_Subtract(turned, &root->unk50, turned);
    KF_Vector_Subtract(moved, root->unk40, moved);
    sceVu0MulVector(moved, moved, &ENTITY_MODEL(entity)->unkE0);
    KF_Vector_Scale(moved, moved, ENTITY_PARAMS_MOTION_SCALE(entity->params));
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
    frame_to = channel->param_shifted;
    swapped = frame_to;
    if (ENTITY_FLAG_BITS(entity)->bit15) {
        frame_to = frame_from;
        frame_from = swapped;
    }
    if (frame_from == frame_to) {
        frame_to = 0x100;
    }
    func_0018D670(entity, entity->unkD1, frame_from, frame_to);
    func_0018D8B8(entity, entity->unkD1, frame_from, frame_to);
    return ENTITY_FLAG_BITS(entity)->bit15 ? channel->param == motion->frame_count : channel->param == 0;
}
