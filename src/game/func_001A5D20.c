#include "kf_effect.h"

s16 func_001B0208(float, void *, void *, void *);

/* Per-frame update for an actor: refresh the sub-state at +0x10 and reset its counters. */
void func_001A5D20(EffectActor *actor, EffectSlot *spawn_slot) {
    TurningMotion *motion;

    motion = (TurningMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &actor->model->rotation);
    motion->unk20 = func_001B0208(30.0f, actor, motion, motion->unk10);
    motion->unk25 = 0;
    motion->unk22 = 0;
    motion->unk24 = 0;
}
