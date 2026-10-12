#include "kf_effect.h"


/* Starts an effect: copies its collision radius and interval, then spawns its start effects. */
void func_001AA230(EffectActor *actor, EffectSlot *spawn_slot) {
    PulseMotion *motion;
    EffectDef *def;

    motion = (PulseMotion *)&actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
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
