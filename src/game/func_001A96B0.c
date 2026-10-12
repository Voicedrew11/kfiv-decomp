#include "kf_effect.h"

void func_001AF298(void *, void *);
void sceVu0AddVector(void *, void *, void *);
void func_001A96B0(EffectActor *actor) {
    FallingMotion *motion;
    func_001AEC48(actor);
    motion = (FallingMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    motion->velocity.y += actor->def->gravity;
    sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
    if (actor->frame > motion->end_frame) {
        if (motion->mode == 5 || motion->mode == 3) {
            func_001A1E28(0x15, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
        }
        func_001A2A18(actor);
        return;
    }
    func_001AF298(actor, motion->unk17);
    actor->frame++;
}
