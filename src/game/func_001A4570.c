#include "common.h"
#include "kf_effect.h"

int rand(void);

// Spawner update: spawns an effect when its frame is due, then schedules the next one.
void func_001A4570(EffectActor *effect) {
    SpawnerMotion *motion = (SpawnerMotion *)&effect->motion;
    ModelInstance *model;

    if (motion->next_spawn_frame < effect->frame) {
        model = effect->model;
        Effect_Spawn(motion->spawn_effect_id, &model->position, &model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
        motion->spawn_count += 1;
        if (motion->spawn_count >= motion->spawn_limit) {
            effect->frame = effect->def->keyframe_end;
        } else {
            effect->frame = 0;
        }
        motion->next_spawn_frame = motion->spawn_frames_base + rand() % motion->spawn_frames_span;
    }
    effect->frame += 1;
}
