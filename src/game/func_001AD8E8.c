#include "kf_effect.h"

#define TWO_PI 6.2831855f
#define PI 3.1415927f

void sceVu0RotMatrixY(void *, void *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
void func_00216C48(void *, void *, void *);

extern f32 D_00431AF0[16];

void func_001AD8E8(EffectActor *effect) {
    SwingMotion *motion;
    EffectDef *def;
    Vector4 spin;
    f32 matrix[16];
    f32 angle;

    func_001AEC48(effect);
    motion = (SwingMotion *)&effect->motion;
    func_001AED58(effect);
    func_001AF0B0(effect);
    motion->counter -= 1;
    if ((s8)motion->counter < 0) {
        if (motion->toggle == 1) {
            motion->toggle = 0;
            motion->counter = effect->def->unkF0;
        } else {
            motion->toggle = 1;
            motion->counter = effect->def->unkF2;
        }
    }
    if (motion->toggle != 0) {
        effect->scale = 0;
    }
    motion->phase = motion->phase + motion->phase_step;  // match: operand order picks retail registers
    spin.y = motion->spin_per_frame * (float)effect->frame;
    spin.z = motion->phase;
    spin.x = 0.0f;
    spin.w = 0.0f;
    angle = motion->angle + effect->def->unkDC;
    angle -= (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    motion->angle = angle;
    sceVu0RotMatrixY(matrix, D_00431AF0, angle);
    sceVu0ApplyMatrix(&spin, matrix, &spin);
    func_00216C48(&effect->model->position, &motion->velocity, &spin);
    effect->frame++;
}
