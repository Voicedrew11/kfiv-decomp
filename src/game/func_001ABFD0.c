#include "common.h"
#include "kf_effect.h"

#define SCRATCH_VECTOR ((void *)0x70000000)
#define SCRATCH_MATRIX_FIRST_WORD (*(int *)0x70000010)

void KF_Vector_Subtract(void *out, void *a, void *b);
void func_00216778(void *dst, void *src);
void func_001C1430(f32, void *, void *);
void func_001AF5B0(void *rotation, void *direction);
float func_001B0178(void *a, void *b);

extern Vector4 D_00413F80;

/* Probe effect: turns its model toward the player and measures the distance to a point. */
void func_001ABFD0(EffectActor *effect, EffectSlot *spawn_slot) {
    TargetDistanceMotion *motion = (TargetDistanceMotion *)&effect->motion;
    Vector4 offset;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion->distance_to_target = 0;
    KF_Vector_Subtract(&offset, &D_00413F80, &effect->model->position);
    func_001C1430(1.0f, &effect->model->position, &offset);
    if (SCRATCH_MATRIX_FIRST_WORD != 0) {
        func_00216778(&motion->unk10, SCRATCH_VECTOR);
    } else {
        func_00216778(&motion->unk10, &D_00413F80);
        motion->unk10.y -= 180.0f;
    }
    KF_Vector_Subtract(&offset, &motion->unk10, &effect->model->position);
    func_001AF5B0(&effect->model->rotation, &offset);
    motion->distance_to_target = func_001B0178(&motion->unk10, &effect->model->position);
    func_001AF538(100.0f, &motion->velocity, &effect->model->rotation);
}
