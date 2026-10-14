#include "kf_effect.h"

void Vector_Copy(void *, void *);

/* Follower effect that copies its target's position into its own motion. */
void func_001AA820(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion;

    motion = &effect->motion;
    func_001AF928(effect, spawn_slot);
    Vector_Copy(motion, &effect->target->model->position);
}
