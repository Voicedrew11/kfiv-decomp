#include "common.h"
#include "kf_effect.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)

int func_001C0E40(f32, void *);
void func_00216778(void *out, void *position);

void func_001A6028(EffectActor *effect, EffectSlot *spawn_slot) {
    ProbeMotion *motion;
    float point[3];

    motion = (ProbeMotion *)&effect->motion;
    func_001AF640(effect, spawn_slot);
    func_001AF928(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    motion->radius = effect->def->unkDC;
    motion->unk14 = 0.0f;
    motion->height = effect->def->unkE0;
    motion->unk1C = effect->def->unkE4;
    motion->unk20 = 1.57f;
    motion->on_ground = 0;
    func_00216778(point, &effect->model->position);
    point[1] -= motion->radius * 1.5f;
    if (func_001C0E40(motion->radius * 3.0f, point) == 0) {
        motion->on_ground = 1;
    } else {
        effect->model->position.y = SCRATCH_VECTOR[1];
    }
}
