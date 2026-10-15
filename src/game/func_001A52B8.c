#include "common.h"
#include "kf_effect.h"

void func_001A52B8(EffectActor *actor, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    ModelInstance *model;

    motion = (TurningMotion *)&actor->motion;
    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    model = actor->model;
    model->position.y -= 220.0f;
    EffectAngle_ToVector(actor->def->speed, &motion->velocity, &model->rotation);
    motion->unk10[0] = 0;
    actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
