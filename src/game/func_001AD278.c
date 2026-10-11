#include "kf_effect.h"

extern char D_00413F80[];

void func_001AF928(void *, int);
void func_001AF640(void *, int);
void func_001AF538(float *, float *, float);
int func_001AFD08(void *, void *, void *);

/* Starts an effect that follows its target, taking the target's speed. */
void func_001AD278(EffectActor *actor, int frame) {
    float speed;
    FollowerMotion *motion;
    EffectActor *target;

    target = actor->target;
    motion = (FollowerMotion *)&actor->motion;
    func_001AF928(actor, frame);
    func_001AF640(actor, frame);
    actor->unk6C = func_001AFD08(actor, D_00413F80, D_00413F80 + 0x10);
    speed = target->motion.velocity.x;
    motion->speed = speed;
    func_001AF538(&motion->velocity.x, &actor->model->rotation.x, speed);
    motion->unk14 = 0;
    motion->unk18 = 1.0f;
    motion->unk20 = 0;
}
