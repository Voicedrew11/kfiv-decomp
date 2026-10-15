#include "common.h"
#include "kf_effect.h"

int EffectHit_FindAimedEntity(EffectActor *effect, Vector4 *position, Vector4 *reference);

/* EffectActor.flags bit 58, set when the effect starts */

/* EffectActor.flags as one 64-bit word, written through a union: a plain s64 store gets reordered */
typedef union {
    u64 word;
    u8 bytes[8];
} EffectFlagWord;

// Lifts the model 220 units, aims it at a target found from its position and starts its velocity.
void func_001A4630(EffectActor *effect, EffectSlot *spawn_slot) {
    HomingMotion *motion = (HomingMotion *)&effect->motion;

    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    effect->model->position.y -= 220.0f;
    effect->unk6C = EffectHit_FindAimedEntity(effect, &effect->model->position, &effect->model->rotation);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->unk14 = 0;
    motion->unkDC = effect->def->unkDC;
    motion->unk18 = 0;
    motion->unk1C = 0;
    motion->unk20 = 0;
    motion->unk24 = 0;
    ((EffectFlagWord *)&effect->flags)->word |= EFFECT_FLAG_HAS_TARGETERS;
}
