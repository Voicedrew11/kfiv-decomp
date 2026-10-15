#include "common.h"
#include "kf_effect.h"

s16 func_001B0208(float, void *, void *, void *);

/* EffectMotion as func_001A8E48 uses it: the start of the velocity and its
   per-kind state with a 16-bit count and a byte flag at 0x20 */

void func_001A8E48(EffectActor *actor, EffectSlot *spawn_slot) {
    CountedMotion *motion;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion = (CountedMotion *)&actor->motion;
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    motion->count = func_001B0208(actor->def->radius_half, actor, motion, &motion->unk10);
    motion->flag = 0;
}
