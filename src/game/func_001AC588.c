#include "common.h"
#include "kf_effect.h"

void func_001C1430(f32, void *, void *);
void func_00216778(void *, void *);

#define SCRATCH_VECTOR ((Vector4 *)0x70000000)
#define SCRATCH_MATRIX ((f32 *)0x70000010)

/* Drops the model by its interval and starts the motion from the heading and speed. */
void func_001AC588(EffectActor *effect, EffectSlot *spawn_slot) {
    EffectDef *def;
    Vector4 *velocity;
    Vector4 offset;

    velocity = &effect->motion.velocity;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    def = effect->def;
    offset.x = 0;
    offset.y = -def->radius_half;
    offset.z = 0;
    offset.w = 0;
    effect->model->position.y -= def->interval;
    func_001C1430(1.0f, &effect->model->position, &offset);
    if (*(int *)SCRATCH_MATRIX != 0) {
        func_00216778(&effect->model->position, SCRATCH_VECTOR);
    } else {
        func_001A2A18(effect);
    }
    velocity->x = 0;
    velocity->y = effect->def->speed;
    velocity->z = 0;
    velocity->w = 0;
}
