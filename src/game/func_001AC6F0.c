#include "kf_effect.h"

extern f32 D_00413F80[];
typedef float Mat[4][4] __attribute__((aligned(16)));

extern Mat D_00431AF0;

int rand(void);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void func_001AF5B0(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void sceVu0RotMatrixY(Mat, Mat, float);
void sceVu0ApplyMatrix(void *, void *, void *);

/* Launches the effect in a random spread around the player's heading, aimed back at the player. */
void func_001AC6F0(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    Mat rotation_matrix;
    Vector4 offset;
    Vector4 player_point;
    f32 heading;

    motion = (TurningMotion *)&effect->motion;
    offset.x = 0.0f;
    offset.y = ((f32)(rand() & 0xFFFF) * 0.000030517578f - 1.0f) * 50.0f;
    offset.y = (f32)(-effect->def->interval) + offset.y;
    offset.z = (f32)effect->def->radius_half;
    offset.w = 0.0f;
    heading = (D_00413F80[5] + effect->def->rotation_base[1]) + (f32)(effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * effect->def->unkDC;
    heading = heading - (f32)(int)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    sceVu0RotMatrixY(rotation_matrix, D_00431AF0, heading);
    sceVu0ApplyMatrix(&offset, rotation_matrix, &offset);
    func_00216C48(&effect->model->position, D_00413F80, &offset);
    func_00216778(&player_point, D_00413F80);
    player_point.y -= 110.0f;
    KF_Vector_Subtract(&offset, &player_point, &effect->model->position);
    func_001AF5B0(&effect->model->rotation, &offset);
    motion->unk20 = (s16)((effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * effect->def->unkF0 + 30);
    motion->unk22 = (s16)(motion->unk20 - effect->def->unkF2);
    motion->unk24 = 0;
}
