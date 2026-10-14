#include "common.h"
#include "kf_effect.h"

extern float D_00413F80[];
extern f32 D_00431AF0[16];

void Vector_Subtract(void *, void *, void *);
int func_001AF358(void *, void *);
void func_001AF5B0(void *, void *);
int func_001AFD08(void *, void *, void *);
void Vector_Copy(void *, void *);
void Vector_Add(void *, void *, void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

void func_001A8218(EffectActor *effect) {
    TimerMotion *motion;
    f32 rotation_matrix[16];
    Vector4 angles;
    Vector4 offset;
    int timer;
    int hit;
    int gone_bits;

    motion = (TimerMotion *)&effect->motion;
    if (effect->unk6C != 0) {
        gone_bits = (int)(((EffectTarget *)effect->unk6C)->flags << 30 >> 32);
        if ((gone_bits & 3) == 0) {
            Vector_Subtract(&offset, &((EffectTarget *)effect->unk6C)->model->position, &effect->model->position);
            offset.y -= ((EffectTarget *)effect->unk6C)->height * 0.5f;
            func_001AFC88(&motion->velocity, &offset, effect->def->speed, effect->def->unkDC);
        } else {
            effect->unk6C = 0;
        }
    }
    if (effect->frame >= effect->def->radius_half) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, motion);
        hit = func_001AF358(effect, &motion->unk24);
        if (hit == 1 && effect->def->interval != 0) {
            motion->timer = effect->def->unkF0;
        }
    } else {
        sceVu0RotMatrixX(rotation_matrix, D_00431AF0, D_00413F80[4]);
        sceVu0RotMatrixY(rotation_matrix, rotation_matrix, D_00413F80[5]);
        sceVu0ApplyMatrix(&offset, rotation_matrix, &motion->unk10);
        Vector_Copy(&effect->model->rotation, &D_00413F80[4]);
        func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
        Vector_Add(&effect->model->position, D_00413F80, &offset);
        effect->model->position.y -= 220.0f;
    }
    timer = motion->timer;
    if (timer > 0) {
        timer -= 1;
        motion->timer = timer;
        if (timer == 0) {
            func_001AF5B0(&angles, motion);
            effect->unk6C = func_001AFD08(effect, &effect->model->position, &angles);
        }
    }
    effect->frame += 1;
}
