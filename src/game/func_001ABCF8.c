#include "kf_effect.h"

void Vector_Add(void *, void *, void *);
void Vector_Scale(float *, float *, float);

/* Turns the model's heading by the caster's level, then moves the model along its velocity scaled by the same level. */
void func_001ABCF8(EffectActor *effect, EffectSlot *spawn_slot) {
    LevelMotion *motion;
    Vector4 offset;
    f32 heading;

    motion = (LevelMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    heading = effect->model->rotation.y + (f32)(effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level) * effect->def->unkDC;
    heading = heading - (f32)(int)(heading / 6.2831855f) * 6.2831855f;
    if (heading > 3.1415927f) {
        heading -= 6.2831855f;
    } else if (heading < -3.1415927f) {
        heading += 6.2831855f;
    }
    effect->model->rotation.y = heading;
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    Vector_Scale(&offset.x, &motion->velocity.x, (f32)(effect->def->max_level - EFFECT_SLOT_FLAG_PARTS(spawn_slot)->level));
    Vector_Add(&effect->model->position, &effect->model->position, &offset);
    motion->scale = 1.0f;
    motion->unk14 = 0;
}
