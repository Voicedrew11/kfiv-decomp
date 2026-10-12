#include "common.h"
#include "kf_effect.h"

extern f32 D_00431AF0[];

void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0RotMatrixZ(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);

/* Launches the effect along the spawner's heading, turned by the spawner's level. */
void func_001A7650(EffectActor *actor, EffectSlot *spawn_slot) {
    ScalingEndMotion *motion;
    f32 matrix[16];
    f32 angle;
    f32 step;

    motion = (ScalingEndMotion *)&actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    actor->motion.velocity.x = motion->velocity.y = motion->velocity.w = 0.0f;
    motion->velocity.z = actor->def->unkE8;
    sceVu0RotMatrixY(matrix, D_00431AF0, spawn_slot->unk10.y);
    sceVu0ApplyMatrix(&motion->end_position.x, matrix, &motion->velocity.x);
    actor->motion.velocity.x = motion->velocity.y = motion->velocity.w = 0.0f;
    motion->velocity.z = actor->def->speed;
    sceVu0RotMatrixX(matrix, D_00431AF0, actor->def->unkE0);
    step = (actor->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * actor->def->unkDC;
    angle = actor->model->rotation.z + step;
    angle -= (int)(angle / 6.2831855f) * 6.2831855f;
    if (angle > 3.1415927f) {
        angle -= 6.2831855f;
    } else if (angle < -3.1415927f) {
        angle += 6.2831855f;
    }
    sceVu0RotMatrixZ(matrix, matrix, angle);
    sceVu0RotMatrixY(matrix, matrix, spawn_slot->unk10.y);
    sceVu0ApplyMatrix(&motion->velocity.x, matrix, &motion->velocity.x);
    motion->scale_per_mille = 1.0f;
}
