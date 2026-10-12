#include "kf_effect.h"

void func_001AF298(void *, void *);
void func_001AF5B0(void *, void *);
void sceVu0AddVector(void *, void *, void *);

void func_001AAF58(EffectActor *actor) {
    DelayedFallMotion *motion;

    func_001AEC48(actor);
    motion = (DelayedFallMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        if (motion->unk18 < actor->frame) {
            motion->velocity.y += actor->def->gravity;
        }
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        func_001AF5B0(&actor->model->rotation, &motion->velocity);
    }
    func_001AF298(actor, &motion->unk1C);
    actor->frame += 1;
}
