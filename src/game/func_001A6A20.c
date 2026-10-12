#include "common.h"
#include "kf_effect.h"

/* Starts an effect that turns at the speed in its definition and clears its flag. */
void func_001A6A20(EffectActor *effect, EffectSlot *spawn_slot) {
    SpeedFlagMotion *motion;

    motion = (SpeedFlagMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->speed = effect->def->unkDC;
    motion->flag14 = 0;
}
