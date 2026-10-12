#include "common.h"
#include "kf_effect.h"

/* Starts an effect that moves at its definition's speed. */
void func_001A3E30(EffectActor *effect, EffectSlot *spawn_slot) {
    TimedMotion *motion;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion = (TimedMotion *)&effect->motion;
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk10 = 0;
    motion->unk14 = 1.0f;
    motion->unk1C = 0;
}
