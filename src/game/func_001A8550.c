#include "common.h"
#include "kf_effect.h"

extern f32 D_00431AF0[16];

void func_001AF5B0(void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
void func_00216778(void *, void *);
int rand(void);

/* Per-frame update: turns the model toward its target and adds a random tilt. */
void func_001A8550(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion;
    f32 rotation_matrix[16];
    Vector4 up_vector;
    Vector4 angles;

    motion = &effect->motion;
    func_001AF640(effect, spawn_slot);
    up_vector.y = 1.0f;
    up_vector.x = up_vector.z = up_vector.w = 0.0f;
    func_001AF5B0(&angles, &effect->target->motion);
    sceVu0RotMatrixX(rotation_matrix, D_00431AF0, angles.x);
    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, angles.y);
    sceVu0ApplyMatrix(&up_vector, rotation_matrix, &up_vector);
    func_001AF5B0(&effect->model->rotation, &up_vector);
    effect->model->rotation.z = effect->model->rotation.z + (effect->def->rotation_base[2] + ((float)(rand() & 0xFFFF) * (1.0f / 32768.0f) - 1.0f) * effect->def->rotation_spread[2]);
    func_00216778(motion, &effect->target->model->position);
    func_00216778(&motion->unk10, &effect->target->model->rotation);
}
