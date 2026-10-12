#include "common.h"
#include "kf_effect.h"

#define TWO_PI 6.2831855f
#define PI 3.1415927f

void func_00216C48(void *, void *, void *);
void *memset(void *, int, u32);

/* Per-frame update of an effect that tilts to and fro and, after radius_half frames, falls ever faster. */
void func_001AB670(EffectActor *effect) {
    Vector4 forward;
    f32 unused_matrix[16]; // match: retail reserves 0x50 bytes of stack it never uses
    Vector4 unused_vector;
    TiltFallMotion *motion;
    f32 angle;

    memset(&forward, 0, 16);
    motion = (TiltFallMotion *)&effect->motion;
    forward.z = 1.0f;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    angle = effect->model->rotation.x + motion->tilt_speed;
    angle -= (float)(int)(angle / TWO_PI) * TWO_PI;
    if (PI < angle) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    effect->model->rotation.x = angle;
    motion->tilt_accel = effect->def->unkDC;
    if (effect->frame < effect->def->radius_half) {
        motion->tilt_accel *= 0.3f;
    }
    if (motion->tilting_back == 0) {
        if (effect->def->unkE0 < effect->model->rotation.x) {
            motion->tilt_speed -= motion->tilt_accel;
        } else {
            motion->tilt_speed += motion->tilt_accel;
            if (0.0f < motion->tilt_speed) {
                motion->tilting_back = 1;
                motion->tilt_speed = 0.0f;
                motion->tilt_accel = -motion->tilt_accel;
            }
        }
    } else {
        if (-effect->def->unkE0 < effect->model->rotation.x) {
            motion->tilt_speed -= motion->tilt_accel;
            if (motion->tilt_speed < 0.0f) {
                motion->tilting_back = 0;
                motion->tilt_speed = 0.0f;
                motion->tilt_accel = -motion->tilt_accel;
            }
        } else {
            motion->tilt_speed += motion->tilt_accel;
        }
    }
    if (effect->def->radius_half < effect->frame) {
        motion->fall_speed += motion->fall_accel;
        motion->velocity.y += motion->fall_speed * motion->fall_speed;
    }
    func_00216C48(&effect->model->position, &motion->velocity, &forward);
    effect->frame++;
}
