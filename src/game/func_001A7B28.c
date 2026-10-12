#include "common.h"
#include "kf_effect.h"

extern EffectActor D_003F3550[];

Entity *func_001AFF78(EffectActor *effect, Vector4 *position);
void func_001A7AC8(EffectActor *effect);
float func_001B0178(void *, void *);
void KF_Vector_Scale(void *, void *, float);
void KF_Vector_Subtract(void *, void *, void *);
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
    func_001AEC48(effect);
    func_001AED58(effect);
    func_001AF0B0(effect);
    if (effect->frame < effect->def->max_frames || effect->def->max_frames == 0) {
        motion->velocity_scale += effect->def->scale_per_frame;
        KF_Vector_Scale(&step, &motion->velocity, motion->velocity_scale);
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
        effect->unk6C = (int)((Entity *(*)(EffectActor *, Vector4 *, Vector4 *))func_001AFF78)(effect, &effect->model->position, &effect->model->rotation);
        motion->turn = 0.0f;
    }
    if (effect->unk6C != 0) {
        if (((s32)(((EffectTarget *)effect->unk6C)->flags >> 2) & 3) == 0) {
            KF_Vector_Subtract(&to_target, &((EffectTarget *)effect->unk6C)->model->position, &effect->model->position);
            to_target.y -= ((EffectTarget *)effect->unk6C)->height * 0.5f;
            if (effect->def->unkE0 < motion->turn) {
                motion->turn = effect->def->unkE0;
            } else {
                motion->turn += 0.1; /* the double is D_0043EF50 */
            }
            func_001AFC88(&motion->velocity, &to_target, motion->turn, 100.0f);
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
        func_001A2A18(effect);
        return;
    }
    if (effect->frame >= 3 && effect->frame < effect->def->keyframe_end - 3 && func_001AF4E0(effect) != 0) {
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
