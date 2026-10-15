#include "common.h"
#include "kf_effect.h"

/* Starts a turning effect, turning at the speed in its definition. */
void func_001A6B88(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;

    motion = (TurningMotion *)&effect->motion;
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk24 = 0;
    motion->unk26 = 0;
    motion->unk25 = 0;
}
