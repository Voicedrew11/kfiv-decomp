#include "common.h"
#include "kf_effect.h"

s16 func_001B0208(float, void *, void *, void *);

/* EffectMotion as func_001A8E48 uses it: the start of the velocity and its
   per-kind state with a 16-bit count and a byte flag at 0x20 */

void func_001A8E48(EffectActor *actor, EffectSlot *spawn_slot) {
    CountedMotion *motion;

    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    motion = (CountedMotion *)&actor->motion;
    func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
    motion->count = func_001B0208(actor->def->radius_half, actor, motion, &motion->unk10);
    motion->flag = 0;
}
