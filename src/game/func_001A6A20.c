#include "common.h"
#include "kf_effect.h"

/* Starts an effect that turns at the speed in its definition and clears its flag. */
void func_001A6A20(EffectActor *effect, EffectSlot *spawn_slot) {
    SpeedFlagMotion *motion;

    motion = (SpeedFlagMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->speed = effect->def->unkDC;
    motion->flag14 = 0;
}
