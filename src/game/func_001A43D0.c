#include "common.h"
#include "kf_effect.h"

// Starts an effect with no end target.
void func_001A43D0(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectStart_TurnModel(effect, spawn_slot);
    EffectStart_PlaceModel(effect, spawn_slot);
    effect->target = NULL;
}
