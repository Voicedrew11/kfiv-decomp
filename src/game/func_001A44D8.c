#include "common.h"
#include "kf_effect.h"

int rand(void);

// Starts a spawner: clears its count and picks the first spawn frame.
void func_001A44D8(EffectActor *effect, EffectSlot *spawn_slot) {
    SpawnerMotion *motion = (SpawnerMotion *)&effect->motion;
    EffectDef *def;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->spawn_count = 0;
    def = effect->def;
    motion->spawn_effect_id = def->radius_half;
    motion->spawn_limit = def->interval;
    motion->spawn_frames_base = def->unkF0;
    motion->spawn_frames_span = def->unkF2;
    motion->next_spawn_frame = motion->spawn_frames_base + rand() % motion->spawn_frames_span;
}
