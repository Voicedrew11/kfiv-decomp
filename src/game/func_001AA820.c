#include "kf_effect.h"

void func_00216778(void *, void *);

/* Follower effect that copies its target's position into its own motion. */
void func_001AA820(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion;

    motion = &effect->motion;
    func_001AF928(effect, spawn_slot);
    func_00216778(motion, &effect->target->model->position);
}
