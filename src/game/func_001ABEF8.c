#include "common.h"
#include "kf_effect.h"

#define SCRATCH_VECTOR ((void *)0x70000000)
#define SCRATCH_MATRIX_FIRST_WORD (*(int *)0x70000010)

void func_00216778(void *dst, void *src);
void func_00216C48(void *dst, void *a, void *b);
void func_001C1430(f32, void *, void *);
float func_001B0178(void *a, void *b);

/* Probe effect: sets its velocity from the model's rotation and measures the distance to a point. */
void func_001ABEF8(EffectActor *effect, EffectSlot *spawn_slot) {
    TargetDistanceMotion *motion = (TargetDistanceMotion *)&effect->motion;
    Vector4 offset;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->distance_to_target = 0;
    func_001AF538(1000.0f, &offset, &effect->model->rotation);
    func_001C1430(1.0f, &effect->model->position, &offset);
    if (SCRATCH_MATRIX_FIRST_WORD != 0) {
        func_00216778(&motion->unk10, SCRATCH_VECTOR);
    } else {
        func_00216C48(&motion->unk10, &offset, &effect->model->position);
    }
    motion->distance_to_target = func_001B0178(&motion->unk10, &effect->model->position);
    func_001AF538(100.0f, &motion->velocity, &effect->model->rotation);
}
