#include "kf_effect.h"

#define EFFECT_POOL_STRIDE 0xA0
/* placeholder: EffectActor.flags bit 56, not yet named */

/* Once the countdown has run out, sets the countdown of every pooled effect of the same kind, then spawns effect 0xDF. */
void func_001A7A48(EffectActor *effect) {
    BobMotion *motion;
    EffectActor *other;
    int i;
    s64 flag;  // match: kept in a register across the loop
    int five;  // match: retail keeps the constant 5 in a register set before the pool index

    motion = (BobMotion *)&effect->motion;
    if (motion->countdown != 0) {
        return;
    }
    flag = EFFECT_FLAG_LIVE;
    five = 5;
    other = D_003F3550;
    for (i = EFFECT_COUNT - 1; i >= 0; i--) {
        if ((other->flags & flag) && other->def->unk100 == effect->def->unk100) {
            ((BobMotion *)&other->motion)->countdown = five;
        }
        other = (EffectActor *)((char *)other + EFFECT_POOL_STRIDE);
    }
    KF_Effect_Spawn(0xDF, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
}
