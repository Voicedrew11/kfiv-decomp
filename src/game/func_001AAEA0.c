#include "common.h"
#include "kf_effect.h"

/* Spawn: launches along the model's rotation at the definition's speed, then sets the radius. */
void func_001AAEA0(EffectActor *effect, EffectSlot *spawn_slot) {
    RadiusMotion *motion = (RadiusMotion *)&effect->motion;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 1.0f;
    motion->radius_half = effect->def->radius_half;
    motion->unk10 = 0;
    motion->unk1C = 0;
}
