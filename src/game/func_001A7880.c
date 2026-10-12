#include "kf_effect.h"

void sceVu0RotMatrixY(void *, void *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
int rand(void);
extern f32 D_00431AF0[];

/* Launches the effect along its model's heading turned by the caller's step count, with random offsets. */
void func_001A7880(EffectActor *actor, EffectSlot *spawn_slot) {
    OffsetSprayMotion *motion;
    f32 rotation[4][4];
    f32 angle;
    f32 *identity;
    EffectDef *def;

    func_001AF928(actor, spawn_slot);
    motion = (OffsetSprayMotion *)&actor->motion;
    func_001AF640(actor, spawn_slot);
    def = actor->def;
    identity = D_00431AF0;
    actor->motion.velocity.x = motion->velocity.y = motion->velocity.w = 0.0f;
    motion->velocity.z = def->speed;
    angle = actor->model->rotation.y + (def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * def->unkDC;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    sceVu0RotMatrixY(rotation, identity, angle);
    sceVu0ApplyMatrix(&motion->velocity, rotation, &motion->velocity);
    actor->flags |= 1L << 58;
    ((EffectActor *)actor->target)->flags &= ~(1L << 58);
    motion->offset_x = (((rand() & 0xFFFF) * 0.000030517578125f) - 1.0f) * 0.5f;
    motion->offset_y = (((rand() & 0xFFFF) * 0.000030517578125f) - 1.0f) * 0.1f + 0.6f;
    motion->unk24 = 0;
    actor->unk6C = 0;
    motion->scale = 1.0f;
    motion->unk23 = 0;
    motion->unk20 = 0;
}
