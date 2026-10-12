#include "kf_effect.h"

extern char D_00413F80[];

int func_001AFD08(void *, void *, void *);

/* Starts an effect that follows its target, taking the target's speed. */
void func_001AD278(EffectActor *actor, EffectSlot *spawn_slot) {
    float speed;
    FollowerMotion *motion;
    EffectActor *target;

    target = actor->target;
    motion = (FollowerMotion *)&actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    actor->unk6C = func_001AFD08(actor, D_00413F80, D_00413F80 + 0x10);
    speed = target->motion.velocity.x;
    motion->speed = speed;
    func_001AF538(speed, &motion->velocity, &actor->model->rotation);
    motion->unk14 = 0;
    motion->unk18 = 1.0f;
    motion->unk20 = 0;
}
