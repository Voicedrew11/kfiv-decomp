#include "kf_effect.h"

extern int D_004468E0;
void func_001AEC48(void *);
void func_001AED58(void *);
void func_001AF0B0(void *);
void func_001AF298(void *, void *);
void func_001A2A18(void *);
int func_001C0E40(float, void *);
void sceVu0AddVector(void *, void *, void *);

void func_001A98D0(EffectActor *actor) {
    EmitterMotion *motion;
    int countdown;
    u16 interval;

    D_004468E0 = 0;
    func_001AEC48(actor);
    motion = (EmitterMotion *)&actor->motion;
    func_001AED58(actor);
    func_001AF0B0(actor);
    countdown = motion->countdown;
    if (countdown <= 0) {
        if (actor->frame < actor->def->max_frames || actor->def->max_frames == 0) {
            sceVu0AddVector(&actor->model->position, &actor->model->position, &motion->velocity);
            if (func_001C0E40(actor->def->radius_half * 2, &actor->model->position) == 0) {
                func_001A2A18(actor);
                return;
            }
            actor->model->position.y = *(float *)0x70000004;
        }
        interval = actor->def->interval;
        motion->countdown = interval;
        countdown = interval;
    }
    motion->countdown = countdown - 1;
    if (actor->frame >= 0x15) {
        func_001AF298(actor, motion->unk18);
    }
    actor->frame += 1;
}
