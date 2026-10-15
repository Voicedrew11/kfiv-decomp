#include "kf_effect.h"
#include "kf_entity.h"

void EffectHit_CheckAndEnd(void *, void *);
void EffectHit_End(void *, void *);
void EffectAngle_FromVector(void *, void *);
float func_001B0970(void *, void *);
void Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Steers the effect toward the entity it chases and moves it. */
void func_001A56F0(EffectActor *actor) {
    FollowerMotion *motion;
    char unused[0x40]; /* unused, but retail reserves the bytes */
    f32 delta[4];
    char unused_tail[0x10]; /* unused, but retail reserves the bytes */

    EffectKeys_ApplyScale(actor);
    motion = (FollowerMotion *)&actor->motion;
    EffectKeys_ApplySpin(actor);
    EffectKeys_ApplyColour(actor);
    if (actor->unk6C != 0) {
        if (((u32)(((Entity *)actor->unk6C)->flags >> 2) & 3) < 2) {
            Vector_Subtract(delta, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
            if (func_001B0970(delta, motion) > 0.0f) {
                motion->speed += actor->def->unkE0;
                delta[1] -= ((Entity *)actor->unk6C)->height * 0.5f;
            } else {
                actor->frame = actor->def->keyframe_end;
            }
            EffectSteer_Toward(&motion->velocity, (Vector4 *)delta, actor->def->speed, motion->speed);
            EffectAngle_FromVector(&actor->model->rotation, motion);
            motion->velocity.w = 0;
        } else {
            actor->unk6C = 0;
        }
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
    }
    EffectHit_CheckAndEnd(actor, &motion->unk14);
    actor->frame++;
    if (actor->frame >= actor->def->keyframe_end) {
        EffectHit_End(actor, &motion->unk14);
    }
}
