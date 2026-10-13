#include "common.h"
#include "kf_effect.h"

extern f32 D_00431AF0[16];
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
void func_00216C48(void *, void *, void *);

/* Spawns an effect at a point offset from the model by the heading rotated by angle. */
void func_001A6498(EffectActor *effect, int effect_id, float angle) {
    Vector4 offset;
    Vector4 spawn_position;
    float rotation_matrix[16];

    offset.z = angle;
    offset.x = 0;
    offset.y = 0;
    offset.w = 0;
    sceVu0RotMatrixY(rotation_matrix, D_00431AF0, effect->model->rotation.y);
    sceVu0ApplyMatrix(&spawn_position.x, rotation_matrix, &offset.x);
    func_00216C48(&spawn_position, &spawn_position, &effect->model->position);
    KF_Effect_SpawnFromEffectAt(effect_id, effect, &spawn_position);
}
