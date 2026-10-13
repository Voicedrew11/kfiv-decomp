#include "common.h"
#include "kf_effect.h"


/* Puts a cooldown on the pool effects of the same kind; once cooled down, spawns effect 0x47. */
void func_001A88A8(EffectActor *effect) {
    EffectActor *other;
    int other_index;

    if (((CooldownMotion *)&effect->motion)->cooldown != 0) {
        return;
    }
    other = D_003F3550;
    for (other_index = 0; other_index < 0x180; other_index++) {
        if (other->flags & EFFECT_FLAG_LIVE) {
            if (other->def->unk100 == effect->def->unk100) {
                ((CooldownMotion *)&other->motion)->cooldown = 5;
            }
        }
        other++;
    }
    KF_Effect_Spawn(0x47, &effect->model->position, &effect->model->rotation, effect->owner_entity, effect->effect_param, effect->owner);
}
