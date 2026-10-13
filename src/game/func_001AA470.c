#include "kf_effect.h"

s16 func_001B0208(float, void *, void *, void *);
extern u8 D_00413FBC[];
extern int D_004468E4;

/* Turns toward a point and spawns def->unkF0 every def->interval frames. */
void func_001AA470(EffectActor *effect, EffectSlot *spawn_slot) {
    TurningMotion *motion;
    s16 angle;

    motion = (TurningMotion *)&effect->motion;
    if (D_00413FBC[0] != 0x26) {
        Effect_FreeSpawner(spawn_slot);
        return;
    }
    func_001AF928(effect, spawn_slot);
    func_001AF640(effect, spawn_slot);
    func_001AF538(effect->def->speed, &motion->velocity, &effect->model->rotation);
    effect->model->position.y -= 220.0f;
    angle = func_001B0208((float)effect->def->radius_half, effect, motion, &motion->unk10);
    motion->unk24 = 0;
    motion->unk22 = angle;
    motion->unk20 = effect->def->end_effect;
    D_004468E4 += 1;
    if (D_004468E4 < effect->def->interval) {
        return;
    }
    Effect_Spawn(effect->def->unkF0, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
    D_004468E4 = 0;
}
