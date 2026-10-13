#include "common.h"
#include "kf_effect.h"

/* EffectActor.flags bit 58, set when the effect spawns */

void func_001A6828(EffectActor *effect, EffectSlot *spawn_slot) {
    SpawnMotion *motion;
    char stack_reserve[0x10]; /* match: retail reserves 0x10 bytes of stack here */

    motion = (SpawnMotion *)&effect->motion;
    func_001AF640(effect, spawn_slot);
    func_001AF928(effect, spawn_slot);
    effect->model->rotation.x = 0.0f;
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->velocity.y = effect->def->unkDC;
    if (effect->def->unkF2 != 0) {
        Effect_SpawnFromEffect(effect->def->unkF2, effect);
    }
    motion->unk10 = 0;
    effect->flags |= EFFECT_FLAG_HAS_TARGETERS;
}
