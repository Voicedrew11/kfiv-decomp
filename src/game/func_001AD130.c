#include "common.h"
#include "kf_effect.h"

/* Starts an effect that follows its target, taking the target's speed. */
void func_001AD130(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectActor *target;
    TargetFollowerMotion *motion;

    target = effect->target;
    motion = (TargetFollowerMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->speed = target->motion.velocity.x;
    func_001AF538(motion->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 0;
    motion->unk18 = 1.0f;
    motion->unk1C = 0;
}
