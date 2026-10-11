#include "kf_effect.h"

void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float, int *, int *);
void sceVu0ApplyMatrix(float *, int *, float *);
extern int D_00431AF0[];

void func_001AEC48(void *);
void func_001AED58(void *);
void func_001AF0B0(void *);
void func_001AF5B0(void *, void *);
void func_00216778(void *, void *);

void func_001A8678(EffectActor *actor) {
    int matrix[16];
    float direction[4];
    float angles[4];
    EffectMotion *motion;

    motion = &actor->motion;
    func_001AEC48(actor);
    func_001AED58(actor);
    func_001AF0B0(actor);
    direction[0] = direction[2] = direction[3] = 0.0f;
    direction[1] = 1.0f;
    func_001AF5B0(angles, &actor->target->motion);
    sceVu0RotMatrixX((float *)matrix, (float *)D_00431AF0, angles[0]);
    sceVu0RotMatrixY(angles[1], matrix, matrix);
    sceVu0ApplyMatrix(direction, matrix, direction);
    func_001AF5B0(&actor->model->rotation, direction);
    func_00216778(&actor->model->position, motion);
    func_00216778(motion, &actor->target->model->position);
    actor->frame += 1;
}
