#include "common.h"
#include "kf_effect.h"

// Lowers the model by 220 units and clears the motion's state.
void func_001A4DB8(EffectActor *effect, EffectSlot *spawn_slot) {
    DropMotion *motion = (DropMotion *)&effect->motion;
    ModelInstance *model;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &model->rotation);
    motion->unk14 = 0;
    motion->unk10 = 0;
}
