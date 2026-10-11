#include "kf_effect.h"

void func_001AF928(void *, int);
void func_001AF640(void *, int);

void func_001ABBE0(EffectActor *actor, int frame) {
    BurstMotion *motion;

    motion = (BurstMotion *)&actor->motion;
    func_001AF928(actor, frame);
    func_001AF640(actor, frame);
    motion->unk1C = 0;
    actor->flags |= 0x8000L << 43;
}
