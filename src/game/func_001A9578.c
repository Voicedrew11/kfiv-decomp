#include "kf_effect.h"


s16 func_001B0208(float, void *, void *, void *);

/* Starts a falling effect and, if no same-kind effect is already live inside its interval, spawns its follow-up. */
void func_001A9578(EffectActor *actor, EffectSlot *spawn_slot) {
    FallingMotion *motion;
    EffectActor *slot;
    int i;
    char scratch[0x10];

    motion = (FallingMotion *)&actor->motion;
    func_001AF928(actor, spawn_slot);
    func_001AF640(actor, spawn_slot);
    func_001AF538(actor->def->speed, &motion->velocity, &actor->model->rotation);
    motion->end_frame = func_001B0208((float)actor->def->radius_half, actor, motion, scratch);
    if (motion->end_frame != 0xFFFF) {
        motion->mode = *(int *)0x70000018;
    }
    motion->unk17[0] = 0;
    motion->spawned = 0;
    slot = D_003F3550;
    for (i = 0; i < 0x180; i++, slot++) {
        if (slot->flags & (0x8000L << 41)) {
            if (slot->def->unk100 == actor->def->unk100 && slot->frame < actor->def->interval) {
                if (((FallingMotion *)&slot->motion)->spawned != 0) {
                    return;
                }
            }
        }
    }
    Effect_Spawn(actor->def->unkF0, &actor->model->position, &actor->model->rotation, actor->owner_entity, actor->effect_param, actor->owner);
    motion->spawned = 1;
}
