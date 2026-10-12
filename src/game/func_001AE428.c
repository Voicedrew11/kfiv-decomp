#include "kf_effect.h"

void func_00216778(void *, void *);

/* Starts an effect that follows its target: copies the target's position and rotation. */
void func_001AE428(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion = &effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_00216778(motion, &effect->target->model->position);
    func_00216778(&effect->motion.unk10, &effect->target->model->rotation);
}
