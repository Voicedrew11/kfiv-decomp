#include "common.h"
#include "kf_effect.h"

extern f32 D_00431AF0[16];

#define TWO_PI 6.2831855f
#define PI 3.14159265f

int func_001AF358(void *, void *);
void func_001AF5B0(void *, void *);
void func_001A88A8(void *);
void sceVu0RotMatrixX(float *, float *, float);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Per-frame update: turns the model's spawn point round its rotation, then ends the effect at end_frame. */
void func_001A8928(EffectActor *effect) {
    CooldownTimerMotion *motion;
    f32 rotation_matrix[16];
    Vector4 forward;
    Vector4 angles;
    float turn_angle;
    float next_angle;
    int hit;
    int step;

    motion = (CooldownTimerMotion *)&effect->motion;
    if (motion->cooldown != 0) {
        motion->cooldown -= 1;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
    }
    if (effect->frame >= motion->end_frame) {
        Effect_Spawn(effect->def->end_effect, &motion->unk10, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        func_001A88A8(effect);
        Effect_Free(effect);
    } else {
        hit = func_001AF358(effect, &motion->unk28);
        if (hit != 0) {
            func_001A88A8(effect);
            if (hit == 1 && effect->def->interval != 0) {
                motion->timer = effect->def->interval;
            }
        }
        if (motion->timer != 0) {
            motion->timer -= 1;
            if (motion->timer == 0) {
                turn_angle = effect->def->unkDC;
                for (step = 0; step < effect->def->radius_half; step++) {
                    forward.x = forward.y = forward.w = 0.0f;
                    forward.z = 1.0f;
                    sceVu0RotMatrixY(rotation_matrix, D_00431AF0, turn_angle);
                    sceVu0RotMatrixX(rotation_matrix, rotation_matrix, effect->model->rotation.x);
                    sceVu0RotMatrixY(rotation_matrix, rotation_matrix, effect->model->rotation.y);
                    sceVu0ApplyMatrix(&forward, rotation_matrix, &forward);
                    func_001AF5B0(&angles, &forward);
                    Effect_Spawn(effect->def->unkF0, &effect->model->position, &angles, effect->owner_entity, effect->effect_param, effect->owner);
                    next_angle = turn_angle + effect->def->unkE0;
                    next_angle -= (float)(int)(next_angle / TWO_PI) * TWO_PI;
                    if (PI < next_angle) {
                        next_angle -= TWO_PI;
                    } else if (next_angle < -PI) {
                        next_angle += TWO_PI;
                    }
                    turn_angle = next_angle;
                }
                Effect_Free(effect);
            }
        }
        effect->frame += 1;
    }
}
