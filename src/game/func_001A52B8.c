#include "common.h"
#include "kf_effect.h"

void func_001A52B8(EffectActor *actor, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    ModelInstance *model;

    motion = (TurningMotion *)&actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    model = actor->model;
    model->position.y -= 220.0f;
    func_001AF538(actor->def->speed, &motion->velocity, &model->rotation);
    motion->unk10[0] = 0;
    actor->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
