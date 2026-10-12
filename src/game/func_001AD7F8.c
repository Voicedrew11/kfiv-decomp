#include "kf_effect.h"

void func_00216778(void *, void *);
int rand(void);

/* Sets up a spiral's start angle, per-frame steps and a random start phase. */
void func_001AD7F8(EffectActor *effect, EffectSlot *spawn_slot) {
    SpiralMotion *motion;
    EffectDef *def;
    ModelInstance *model;
    int random_step;

    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    motion = (SpiralMotion *)&effect->motion;
    func_00216778(motion, &effect->model->position);
    model = effect->model;
    def = effect->def;
    motion->unk14 = 0;
    motion->start_angle = model->rotation.y + def->unkE0;
    motion->unk18 = (f32)def->radius_half / ((f32)def->keyframe_end * 0.5f);
    motion->unk1C = (f32)def->interval / (f32)def->keyframe_end;
    random_step = rand() % 10;
    motion->unk21 = 0;
    motion->start_step = (u8)random_step;
}
