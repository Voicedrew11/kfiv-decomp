#include "kf_effect.h"

int func_001A1E28(int, int, int, int, u16, int);
int rand(void);

void func_001A4570(EffectActor *actor) {
    SpawnerMotion *motion;

    motion = (SpawnerMotion *)&actor->motion;
    if (motion->next_frame < actor->frame) {
        func_001A1E28(motion->effect_id, (int)&actor->model->position, (int)&actor->model->rotation, actor->effect_arg, actor->effect_param, (int)actor->owner);
        motion->count++;
        if (motion->count >= motion->repeat_limit) {
            actor->frame = actor->def->restart_frame;
        } else {
            actor->frame = 0;
        }
        motion->next_frame = motion->interval_base + rand() % motion->interval_range;
    }
    actor->frame++;
}
