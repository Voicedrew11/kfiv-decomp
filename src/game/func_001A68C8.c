#include "kf_effect.h"

void func_001AF5B0(void *, void *);
int func_001B4F90(void *);
void sceVu0AddVector(void *, void *, void *);

/* Falls under gravity while inside max_frames; after frame 20 it ends on a hit, spawning one or two effects. */
void func_001A68C8(EffectActor *actor) {
    LiftedMotion *motion;
    u16 end_effect;
    char pad[0x10]; /* unused, but retail reserves the 0x10 bytes */

    func_001AEC48(actor);
    motion = (LiftedMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        motion->velocity.y += actor->def->gravity;
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        func_001AF5B0(&actor->model->rotation, &motion->velocity);
    }
    if (actor->frame < 0x14) {
        if (motion->unk10 != 0) {
            Effect_Free(actor);
        } else if (func_001B4F90(actor) != 0) {
            if (*(int *)0x70000018 == 3 || *(int *)0x70000018 == 5) {
                end_effect = actor->def->unkF0;
                Effect_Spawn(0x122, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            } else {
                end_effect = actor->def->end_effect;
            }
            Effect_Spawn(end_effect, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
            motion->unk10 = 1;
        }
    }
    actor->frame++;
}
