#include "common.h"
#include "kf_effect.h"
#include "kf_player.h"

#define TWO_PI 6.2831855f
#define PI 3.1415927f

extern PlayerState D_00413F80;
extern f32 D_00431AF0[16];

int func_001B4F90(void *);
float func_001B0178(void *, void *);
void func_001AF5B0(void *, void *);
void func_00216778(void *, void *);
void func_00216C48(void *, void *, void *);
void KF_Vector_Scale(void *, void *, float);
void KF_Vector_Subtract(void *, void *, void *);
void sceVu0RotMatrixY(float *, float *, float);
void sceVu0ApplyMatrix(void *, void *, void *);

/* Wraps an angle into -pi..pi. */
static inline f32 wrap_angle(f32 angle) {
    angle = angle - (float)(int)(angle / TWO_PI) * TWO_PI;
    if (angle > PI) {
        angle -= TWO_PI;
    } else if (angle < -PI) {
        angle += TWO_PI;
    }
    return angle;
}

/* Per-frame update of a sweeping effect: at its arrival frame it spawns the end effect at its aim,
   then the aim sweeps along the ground on a widening spiral, spawning a hit effect where it finds something. */
void func_001A92A8(EffectActor *effect) {
    f32 matrix[16];
    Vector4 to_aim;
    Vector4 saved_position;
    SweepMotion *motion;
    int hits;

    motion = (SweepMotion *)&effect->motion;
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame == motion->arrival_frame) {
        if (motion->has_aim != 0) {
            if (motion->first_level != 0) {
                func_001A1E28(effect->def->end_effect, &motion->aim, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
            }
            if (motion->on_ground != 0) {
                effect->motion.velocity.x = 0.0f;
                motion->velocity.z = 50.0f;
                motion->velocity.y = 0.0f;
                motion->velocity.w = 0.0f;
                motion->sweep_turn = effect->def->keys_b[3].y;
                sceVu0RotMatrixY(matrix, D_00431AF0, wrap_angle(D_00413F80.rotation.y + effect->def->unkDC + effect->def->unkE0 * (f32)motion->level_step));
                sceVu0ApplyMatrix(&motion->velocity, matrix, &motion->velocity);
            }
        }
        motion->arrived = 1;
    }
    if (motion->arrived != 0) {
        if (motion->on_ground != 0 && motion->arrival_frame + 10 < effect->frame) {
            sceVu0RotMatrixY(matrix, D_00431AF0, motion->sweep_turn);
            sceVu0ApplyMatrix(&motion->velocity, matrix, &motion->velocity);
            KF_Vector_Scale(&motion->velocity, &motion->velocity, 1.02f);
            func_00216C48(&motion->aim, &motion->aim, &motion->velocity);
            KF_Vector_Subtract(&to_aim, &motion->aim, &effect->model->position);
            func_001AF5B0(&effect->model->rotation, &to_aim);
            if (motion->hit_spawned == 0) {
                func_00216778(&saved_position, &effect->model->position);
                func_00216778(&effect->model->position, &motion->aim);
                hits = func_001B4F90(effect);
                func_00216778(&effect->model->position, &saved_position);
                if (hits != 0) {
                    func_001A1E28(effect->def->unkF0, &motion->aim, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                    motion->hit_spawned = 1;
                }
            }
        }
        effect->model->unkE8 = func_001B0178(&motion->aim, &effect->model->position) / 100.0f + 1.0f;
    }
    effect->frame++;
}
