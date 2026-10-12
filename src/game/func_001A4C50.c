#include "common.h"
#include "kf_effect.h"

s16 func_001B0208(float, void *, void *, void *);

// Lifts the model by 220 units, starts its motion and stores func_001B0208's result.
void func_001A4C50(EffectActor *effect, EffectSlot *spawn_slot) {
    LaunchMotion *motion = (LaunchMotion *)&effect->motion;
    ModelInstance *model;
    float scratch[3];

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
    func_001AF538(effect->def->speed, &motion->velocity, &model->rotation);
    motion->unk10 = func_001B0208((float)effect->def->radius_half, effect, motion, scratch);
}
