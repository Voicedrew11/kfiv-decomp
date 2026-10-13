#include "kf_effect.h"
#include "kf_entity.h"

void func_001AF298(void *, void *);
void func_001AF5B0(void *, void *);
float func_001B0970(void *, void *);
void Vector_Subtract(void *, void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* Steers toward the entity it chases, ending the chase once that entity has flag bits 2-3 set. */
void func_001A8CF0(EffectActor *actor) {
    FollowerMotion *motion;
    char unused[0x40]; /* unused, but retail reserves the bytes */
    f32 delta[4];
    char unused_tail[0x10]; /* unused, but retail reserves the bytes */

    func_001AEC48(actor);
    motion = (FollowerMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (actor->unk6C != 0) {
        if (ENTITY_FLAG_BITS((Entity *)actor->unk6C)->bits2 == 0) {
            Vector_Subtract(delta, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
            if (func_001B0970(delta, motion) > 0.0f) {
                motion->speed += actor->def->unkE0;
                delta[1] -= ((Entity *)actor->unk6C)->height * 0.5f;
            }
            func_001AFC88(&motion->velocity, (Vector4 *)delta, actor->def->speed, motion->speed);
            func_001AF5B0(&actor->model->rotation, motion);
            motion->velocity.w = 0;
        } else {
            actor->unk6C = 0;
        }
    }
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        sceVu0AddVector(&actor->model->position, &actor->model->position, motion);
    }
    func_001AF298(actor, &motion->unk14);
    actor->frame++;
}
