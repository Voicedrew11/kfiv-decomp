#include "kf_effect.h"

void func_001AF928(void *, int);
void func_001AF640(void *, int);
s32 func_001A2838(u16, void *);

/* Starts an effect: copies its collision radius and interval, then spawns its start effects. */
void func_001AA230(EffectActor *actor, int frame) {
    PulseMotion *motion;
    EffectDef *def;

    motion = (PulseMotion *)&actor->motion;
    func_001AF928(actor, frame);
    func_001AF640(actor, frame);
    def = actor->def;
    motion->radius_half = def->radius_half;
    motion->interval = def->interval;
    if (def->unkF0 != 0) {
        func_001A2838(def->unkF0, actor);
    }
    if (actor->def->unkF2 != 0) {
        func_001A2838(actor->def->unkF2, actor);
    }
}
