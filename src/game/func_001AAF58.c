#include "kf_effect.h"

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
void func_001AF298(void *, void *);
void func_001AF5B0(void *, void *);
void sceVu0AddVector(void *, void *, void *);

/* applies gravity to the velocity after fall_frame, then moves the model */
void func_001AAF58(EffectActor *actor) {
    GravityMotion *motion;

    func_001AEC48(actor);
    motion = (GravityMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
        if (motion->fall_frame < actor->frame) {
            motion->velocity.y += actor->def->gravity;
        }
        sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
        func_001AF5B0(&actor->model->rotation, motion);
    }
    func_001AF298(actor, motion->unk1C);
    actor->frame++;
}
