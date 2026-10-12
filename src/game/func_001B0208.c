#include "kf_effect.h"

#define SCRATCH_VECTOR ((void *)0x70000000)
#define SCRATCH_MATRIX ((int *)0x70000010)

float func_001B0178(void *, void *);
void func_001C1430(f32, void *, void *);
void func_00216778(void *, void *);

/* the inline twin of func_001B0950 */
static inline f32 absolute_value(f32 value) {
    if (value < 0.0f) {
        return -value;
    }
    return value;
}

/* Casts the effect's path forward; stores where it lands in hit_point and returns the
   number of frames to get within radius of it, or -1 if it hits nothing. */
s16 func_001B0208(float radius, void *effect, void *motion, void *hit_point) {
    EffectActor *actor = effect;
    f32 distance;
    f32 stack_reserve[4]; // match: unused, keeps the retail stack frame size

    func_001C1430(actor->def->keyframe_end, &actor->model->position, motion);
    if (*SCRATCH_MATRIX != 0) {
        distance = func_001B0178(SCRATCH_VECTOR, &actor->model->position);
        func_00216778(hit_point, SCRATCH_VECTOR);
        return absolute_value((distance - radius) / actor->def->speed);
    }
    return -1;
}
