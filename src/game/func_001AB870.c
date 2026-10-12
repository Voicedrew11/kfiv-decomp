#include "kf_effect.h"

int func_001C0E40(f32, void *);
int rand(void);

/* Sinks the model by its definition's float, then sets a random countdown to the next check. */
void func_001AB870(EffectActor *actor, EffectSlot *spawn_slot) {
    CountdownMotion *motion;

    motion = (CountdownMotion *)&actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    actor->model->position.y -= actor->def->unkDC;
    if (func_001C0E40(actor->def->unkDC * 2.0f, &actor->model->position) == 1) {
        actor->model->position.y = *(float *)0x70000004 - actor->def->unkDC;
    } else {
        func_001A2A18(actor);
    }
    motion->countdown = actor->def->radius_half + rand() % actor->def->interval;
}
