#include "common.h"
#include "kf_effect.h"

s16 func_001B0208(float, void *, void *, void *);

// Lifts the model by 220 units, starts its motion and stores func_001B0208's result.
void func_001A4C50(EffectActor *effect, EffectSlot *spawn_slot) {
    LaunchMotion *motion = (LaunchMotion *)&effect->motion;
    ModelInstance *model;
    float scratch[3];

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &model->rotation);
    motion->unk10 = func_001B0208((float)effect->def->radius_half, effect, motion, scratch);
}
