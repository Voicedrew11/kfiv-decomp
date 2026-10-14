#include "kf_effect.h"

void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(float *, float *, float *);
extern f32 D_00431AF0[16];

void func_001AF5B0(void *, void *);
void Vector_Copy(void *, void *);

void func_001A8678(EffectActor *actor) {
    f32 matrix[16];
    Vector4 direction;
    Vector4 angles;
    EffectMotion *motion;

    motion = &actor->motion;
    func_001AEC48(actor);
    func_001AED58(actor);
    func_001AF0B0(actor);
    direction.x = direction.z = direction.w = 0.0f;
    direction.y = 1.0f;
    func_001AF5B0(&angles, &actor->target->motion);
    sceVu0RotMatrixX(matrix, D_00431AF0, angles.x);
    sceVu0RotMatrixY(matrix, matrix, angles.y);
    sceVu0ApplyMatrix(&direction.x, matrix, &direction.x);
    func_001AF5B0(&actor->model->rotation, &direction);
    Vector_Copy(&actor->model->position, motion);
    Vector_Copy(motion, &actor->target->model->position);
    actor->frame += 1;
}
