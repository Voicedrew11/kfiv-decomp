#include "common.h"
#include "kf_effect.h"

int func_001A6BF8(EffectActor *effect, float *position, AimMotion *motion);
float func_001B0178(void *, void *);
void func_00216778(void *, void *);
void KF_Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Per-frame update of an effect that trails the effect it is aimed at; once that one has ended it homes on
   the ground below it and spawns the end effect on arrival. */
void func_001A6C98(EffectActor *effect) {
    Vector4 to_aim;
    AimMotion *motion;
    SpeedFlagMotion *target_motion;
    Vector4 *aim;
    f32 distance;
    s8 reached;

    motion = (AimMotion *)&effect->motion;
    target_motion = (SpeedFlagMotion *)&effect->target->motion;
    if (motion->ended == 1) {
        func_001A2A18(effect);
        return;
    }
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (motion->aimed == 0 && (s8)target_motion->flag14 != 0) {
        motion->aimed = 1;
        if (!func_001A6BF8(effect, &effect->target->model->position.x, motion) && !func_001A6BF8(effect, &effect->model->position.x, motion)) {
            func_00216778(&motion->target_x, &effect->target->model->position);
            effect->frame = effect->def->keyframe_end - 4;
        }
    }
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        aim = (Vector4 *)&motion->target_x;
        distance = func_001B0178(&effect->model->position, aim);
        if (motion->aimed == 0) {
            KF_Vector_Subtract(&to_aim, &effect->target->model->position, &effect->model->position);
            motion->turn = 3000.0f / distance * 8.0f;
            if (motion->turn < 8.0f) {
                motion->turn = 8.0f;
            }
        } else {
            KF_Vector_Subtract(&to_aim, aim, &effect->model->position);
            motion->turn = 65535.0f;
        }
        if (effect->frame >= 13) {
            func_001AFC88(&motion->velocity, &to_aim, effect->def->speed, motion->turn);
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        if (motion->aimed == 1) {
            reached = motion->flag25; // match: a local keeps this test apart from the one on aimed
            if (reached == 1 && distance < 50.0f) {
                func_001A1E28(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                motion->ended = 1;
            }
        }
    }
    effect->frame++;
}
