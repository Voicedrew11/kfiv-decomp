#include "common.h"
#include "kf_effect.h"

int func_001A6BF8(EffectActor *effect, float *position, AimMotion *motion);
float func_001B0178(void *, void *);
void Vector_Copy(void *, void *);
void Vector_Subtract(void *, void *, void *);
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
        Effect_Free(effect);
        return;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (motion->aimed == 0 && (s8)target_motion->flag14 != 0) {
        motion->aimed = 1;
        if (!func_001A6BF8(effect, &effect->target->model->position.x, motion) && !func_001A6BF8(effect, &effect->model->position.x, motion)) {
            Vector_Copy(&motion->target_x, &effect->target->model->position);
            effect->frame = effect->def->keyframe_end - 4;
        }
    }
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        aim = (Vector4 *)&motion->target_x;
        distance = func_001B0178(&effect->model->position, aim);
        if (motion->aimed == 0) {
            Vector_Subtract(&to_aim, &effect->target->model->position, &effect->model->position);
            motion->turn = 3000.0f / distance * 8.0f;
            if (motion->turn < 8.0f) {
                motion->turn = 8.0f;
            }
        } else {
            Vector_Subtract(&to_aim, aim, &effect->model->position);
            motion->turn = 65535.0f;
        }
        if (effect->frame >= 13) {
            EffectSteer_Toward(&motion->velocity, &to_aim, effect->def->speed, motion->turn);
        }
        sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        if (motion->aimed == 1) {
            reached = motion->flag25; // match: a local keeps this test apart from the one on aimed
            if (reached == 1 && distance < 50.0f) {
                Effect_Spawn(effect->def->end_effect, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
                motion->ended = 1;
            }
        }
    }
    effect->frame++;
}
