#include "common.h"
#include "kf_effect.h"

/* Starts a turning effect, turning at the speed in its definition. */
void func_001A6B88(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;

    motion = (TurningMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk24 = 0;
    motion->pad26[0] = 0;
    motion->unk25 = 0;
}
