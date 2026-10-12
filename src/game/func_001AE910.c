#include "common.h"
#include "kf_effect.h"

void func_001AE910(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion;

    motion = &effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
}
