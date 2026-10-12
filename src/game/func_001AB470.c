#include "common.h"
#include "kf_effect.h"

/* Spawn: moves along the model's heading at the definition's speed. */
void func_001AB470(EffectActor *effect, EffectSlot *spawn_slot) {
    HeadingMotion *motion = (HeadingMotion *)&effect->motion;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk10 = 0;
}
