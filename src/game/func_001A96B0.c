#include "kf_effect.h"

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);  // match: callee return value must be visible (int) for retail register allocation; other callers differ
void func_001AF298(void *, void *);
void sceVu0AddVector(void *, void *, void *);
void func_001A2A18(void *);
int func_001A1E28(int, int, int, int, u16, int);

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
            func_001A1E28(0x15, (int)(&actor->model->position), (int)(&actor->model->rotation), actor->effect_arg, actor->effect_param, (int)actor->owner);
        }
        func_001A2A18(actor);
        return;
    }
    func_001AF298(actor, motion->unk17);
    actor->frame++;
}
