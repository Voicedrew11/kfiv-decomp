#include "common.h"
#include "kf_effect.h"


/* Lowers the effect's model 220 units in y after the common update. */
void func_001A3EA8(EffectActor *effect, EffectSlot *spawn_slot) {
    ModelInstance *model;

    func_001A3E30(effect, spawn_slot);
    model = effect->model;
    model->position.y -= 220.0f;
}
