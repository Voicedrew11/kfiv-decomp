#include "kf_effect.h"

s16 func_001B0208(float, void *, void *, void *);

/* Turning effect launched along its model's heading; its end frame and turn come from the definition. */
void func_001A9E88(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    s16 angle;
    float unk_e8;

    motion = (TurningMotion *)&effect->motion;
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &effect->motion.velocity, &effect->model->rotation);
    angle = func_001B0208(effect->def->unkDC, effect, motion, &motion->unk10);
    motion->unk22 = angle;
    if (effect->def->unkF0 < (u32)(angle & 0xFFFF)) {
        *(u16 *)&motion->unk22 = 0xFFFF; // match: stored as an unsigned halfword
    }
    unk_e8 = effect->def->unkE8;
    motion->unk25 = 0;
    motion->unk24 = (int)unk_e8;
    motion->unk20 = effect->def->end_effect;
}
