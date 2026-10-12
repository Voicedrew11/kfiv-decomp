#include "common.h"
#include "kf_entity.h"

void func_00216778(void *, void *);

/* Starts the pool point at index easing toward its target over duration frames. */
void func_0019B1E8(f32 duration, s32 index, void *actor) {
    LinkedPoint *point;

    if ((u32)index < LINKED_POINT_COUNT) {
        point = &D_003E7500[index];
        point->duration = duration;
        point->position.w = 1.0f / duration;
        point->progress_scale = 1.0f / (duration * duration);
        func_00216778(&point->target, actor);
        point->target.w = 1.0f;
    }
}
