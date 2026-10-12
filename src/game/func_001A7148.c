#include "kf_effect.h"
#include "kf_entity.h"

#define SCRATCH_VECTOR ((f32 *)0x70000000)
#define SCRATCH_MATRIX ((int *)0x70000010)

void func_001A7110(void *, void *, void *);
float func_001B0178(void *, void *);
void func_001C1430(f32, void *, void *);
void func_00216778(void *, void *);
int rand(void);

/* Starts an effect that aims at the entity its caller's slot picked, or at a random point around it. */
void func_001A7148(EffectActor *actor, EffectSlot *spawn_slot) {
    ScalingEndMotion *motion = (ScalingEndMotion *)&actor->motion;
    PickedMotion *picked = (PickedMotion *)&actor->target->motion;
    Entity *chosen;
    f32 offset[4];

    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    motion->ended = 0;
    motion->scale_per_mille = 0;
    chosen = picked->entity[EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level - 1];
    if (chosen != NULL) {
        actor->unk6C = (int)chosen;
        motion->scale_per_mille = picked->distance[EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level - 1];
        func_00216778(&motion->end_position, chosen->model->position);
        func_001A7110(&actor->model->rotation, ((Entity *)actor->unk6C)->model->position, &actor->model->position);
        return;
    }
    offset[0] = ((rand() & 0xFFFF) * 3.0517578e-05f - 1.0f) * 0.7f;
    offset[1] = 1.0f;
    offset[2] = ((rand() & 0xFFFF) * 3.0517578e-05f - 1.0f) * 0.7f;
    func_001C1430(1500.0f, &actor->model->position, offset);
    if (*SCRATCH_MATRIX != 0) {
        func_001A7110(&actor->model->rotation, SCRATCH_VECTOR, &actor->model->position);
        motion->scale_per_mille = func_001B0178(SCRATCH_VECTOR, &actor->model->position);
        func_00216778(&motion->end_position, SCRATCH_VECTOR);
    } else {
        motion->ended = 1;
    }
}
