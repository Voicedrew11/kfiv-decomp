#include "common.h"
#include "kf_effect.h"


Entity *EffectHit_FindNearestEntity(EffectActor *effect, Vector4 *position, Vector4 *unused_rotation);
void func_001A7AC8(EffectActor *effect);
float func_001B0178(void *, void *);
void Vector_Scale(void *, void *, float);
void Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Per-frame update of a bobbing effect that speeds up along its heading, then homes on the nearest entity;
   when it ends, the other effects of its group end one after another, by distance. */
void func_001A7B28(EffectActor *effect) {
    Vector4 step;
    Vector4 to_target;
    HomingBobMotion *motion;
    EffectActor *other;
    int i;

    motion = (HomingBobMotion *)&effect->motion;
    if (motion->countdown != 0) {
        motion->countdown--;
    }
    EffectKeys_ApplyScale(effect);
    EffectKeys_ApplySpin(effect);
    EffectKeys_ApplyColour(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        motion->velocity_scale += effect->def->scale_per_frame;
        Vector_Scale(&step, &motion->velocity, motion->velocity_scale);
        sceVu0AddVector(&effect->model->position, &effect->model->position, &step);
    }
    motion->bob_speed += motion->bob_rising != 0 ? 0.01f : -0.01f;
    if (motion->bob_limit < motion->bob_speed) {
        motion->bob_rising = FALSE;
    }
    if (motion->bob_speed < -motion->bob_limit) {
        motion->bob_rising = TRUE;
    }
    effect->model->position.y += motion->bob_speed;
    if (effect->frame == effect->def->max_frames && effect->def->interval != 0) {
        // match: retail passes the rotation as a third argument the function does not take
        effect->unk6C = (int)EffectHit_FindNearestEntity(effect, &effect->model->position, &effect->model->rotation);
        motion->turn = 0.0f;
    }
    if (effect->unk6C != 0) {
        if (((s32)(((EffectTarget *)effect->unk6C)->flags >> 2) & 3) == 0) {
            Vector_Subtract(&to_target, &((EffectTarget *)effect->unk6C)->model->position, &effect->model->position);
            to_target.y -= ((EffectTarget *)effect->unk6C)->height * 0.5f;
            if (effect->def->unkE0 < motion->turn) {
                motion->turn = effect->def->unkE0;
            } else {
                motion->turn += 0.1; /* the double is D_0043EF50 */
            }
            EffectSteer_Toward(&motion->velocity, &to_target, motion->turn, 100.0f);
            sceVu0AddVector(&effect->model->position, &effect->model->position, &motion->velocity);
        } else {
            effect->unk6C = 0;
        }
    }
    if (motion->ended != 0) {
        other = D_003F3550;
        for (i = 0; i < 0x180; i++, other++) {
            if ((other->flags & EFFECT_FLAG_LIVE) && other->target == effect->target && ((HomingBobMotion *)&other->motion)->end_countdown == 0 && other != effect) {
                ((HomingBobMotion *)&other->motion)->end_countdown = (int)(func_001B0178(&other->model->position, &effect->model->position) / 20.0f);
            }
        }
        Effect_Free(effect);
        return;
    }
    if (effect->frame >= 3 && effect->frame < effect->def->keyframe_end - 3 && EffectHit_Check(effect) != 0) {
        func_001A7AC8(effect);
    }
    if (motion->ended == 0) {
        if (motion->end_countdown > 0) {
            motion->end_countdown--;
            if (motion->end_countdown == 0) {
                func_001A7AC8(effect);
            }
        }
        if (motion->ended == 0 && effect->frame == effect->def->keyframe_end - 1) {
            func_001A7AC8(effect);
        }
    }
    effect->frame++;
}
