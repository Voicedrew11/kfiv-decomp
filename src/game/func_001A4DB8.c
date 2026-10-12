#include "common.h"
#include "kf_effect.h"

// Lowers the model by 220 units and clears the motion's state.
void func_001A4DB8(EffectActor *effect, EffectSlot *spawn_slot) {
    DropMotion *motion = (DropMotion *)&effect->motion;
    ModelInstance *model;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
    func_001AF538(effect->def->speed, &motion->velocity, &model->rotation);
    motion->unk14 = 0;
    motion->unk10 = 0;
}
