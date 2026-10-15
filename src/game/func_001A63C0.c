#include "common.h"
#include "kf_effect.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)

int func_001C0E40(f32, void *);
void Vector_Copy(void *out, void *position);

void func_001A63C0(EffectActor *effect, EffectSlot *spawn_slot) {
    ProbeFlatMotion *motion;
    float point[3];

    motion = (ProbeFlatMotion *)&effect->motion;
    EffectStart_PlaceModel(effect, spawn_slot);
    EffectStart_TurnModel(effect, spawn_slot);
    EffectAngle_ToVector(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->radius = effect->def->unkDC;
    motion->unk14 = 0.0f;
    motion->on_ground = 0;
    Vector_Copy(point, &effect->model->position);
    point[1] -= motion->radius * 1.5f;
    if (func_001C0E40(motion->radius * 3.0f, point) == 0) {
        motion->on_ground = 1;
    } else {
        effect->model->position.y = SCRATCH_VECTOR[1];
    }
}
