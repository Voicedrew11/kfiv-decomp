#include "kf_effect.h"

int rand(void);

/* Starts an effect at a random point inside the box the definition describes, and lifts the model 220 units. */
void func_001A80E8(EffectActor *actor, EffectSlot *spawn_slot) {
    EndAtFrameMotion *motion;

    EffectStart_TurnModel(actor, spawn_slot);
    EffectStart_PlaceModel(actor, spawn_slot);
    motion = (EndAtFrameMotion *)&actor->motion;
    motion->end_position.x = actor->def->offset_base[0] + ((rand() & 0xFFFF) * (1.0f / 32768) - 1.0f) * actor->def->offset_spread[0];
    motion->end_position.y = actor->def->offset_base[1] + ((rand() & 0xFFFF) * (1.0f / 32768) - 1.0f) * actor->def->offset_spread[1];
    motion->end_position.z = actor->def->offset_base[2] + ((rand() & 0xFFFF) * (1.0f / 32768) - 1.0f) * actor->def->offset_spread[2];
    motion->end_position.w = 0;
    motion->end_frame = -1;
    actor->model->position.y -= 220.0f;
    motion->ended = 0;
    actor->unk6C = 0;
    actor->flags |= (s64)0x8000 << 0x2B;
}
