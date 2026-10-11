#include "kf_effect.h"

void func_001AF928(void *, int);
void func_001AF640(void *, int);
void func_001AF538(float *, float *, float);
s16 func_001B0208(float, void *, void *, void *);

/* Per-frame update for an actor: refresh the sub-state at +0x10 and reset its counters. */
void func_001A5D20(EffectActor *actor, int frame) {
    TurningMotion *motion;

    motion = (TurningMotion *)&actor->motion;
    func_001AF928(actor, frame);
    func_001AF640(actor, frame);
    func_001AF538(&motion->velocity.x, &actor->model->rotation.x, actor->def->speed);
    motion->unk20 = func_001B0208(30.0f, actor, motion, motion->unk10);
    motion->unk25 = 0;
    motion->unk22 = 0;
    motion->unk24 = 0;
}
