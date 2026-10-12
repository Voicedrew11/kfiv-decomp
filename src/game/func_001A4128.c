#include "common.h"
#include "kf_effect.h"

extern char D_00413F80[];

int func_001AFD08(void *, void *, void *);
/* Starts an effect at a target's position, or at its own model if it has none. */
void func_001A4128(EffectActor *effect, EffectSlot *spawn_slot) {
    TargetMotion *motion;
    EffectTarget *target;
    int target_id;

    target_id = func_001AFD08(effect, D_00413F80, D_00413F80 + 0x10);
    effect->unk6C = target_id;
    motion = (TargetMotion *)&effect->motion;
    if (target_id == 0) {
        func_001AF640(effect, spawn_slot);
        func_001AF928(effect, spawn_slot);
    } else {
        target = (EffectTarget *)target_id;
        effect->model->position.x = target->model->position.x;
        effect->model->position.y = target->model->position.y - target->height * 0.5f;
        effect->model->position.z = target->model->position.z;
        effect->model->rotation.x = 0.0f;
        effect->model->rotation.y = 0.0f;
        effect->model->rotation.z = 0.0f;
    }
    motion->unk00 = effect->def->radius_half;
    motion->unk04 = 0;
}
