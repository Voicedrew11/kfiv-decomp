#include "kf_effect.h"

void Vector_Copy(void *, void *);

/* Starts an effect that follows its target: copies the target's position and rotation. */
void func_001AE428(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectMotion *motion = &effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    Vector_Copy(motion, &effect->target->model->position);
    Vector_Copy(&effect->motion.unk10, &effect->target->model->rotation);
}
