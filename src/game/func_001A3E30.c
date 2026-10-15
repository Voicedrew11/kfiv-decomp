#include "common.h"
#include "kf_effect.h"

/* Starts an effect that moves at its definition's speed. */
void func_001A3E30(EffectActor *effect, EffectSlot *spawn_slot) {
    TimedMotion *motion;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    motion = (TimedMotion *)&effect->motion;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk10 = 0;
    motion->unk14 = 1.0f;
    motion->unk1C = 0;
}
