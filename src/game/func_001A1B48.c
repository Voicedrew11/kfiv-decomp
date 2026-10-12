#include "common.h"
#include "kf_effect.h"

extern EffectSlot D_003F1750[0x60];
extern EffectActor D_003F3550[0x180];
extern int D_004468CC;
extern int D_004468D0;

void func_00216C48(void *, void *, void *);

/* Transforms the positions of the live effect slots and effect actors by a matrix. */
void func_001A1B48(void *transform) {
    s64 slot_in_use_bit;
    s64 actor_in_use_bit;
    EffectSlot *slot;
    EffectActor *effect;
    Vector4 *model_position;
    int pool_index;
    int live_left;

    slot_in_use_bit = EFFECT_SLOT_LIVE;
    slot = D_003F1750;
    effect = D_003F3550;
    live_left = D_004468CC;
    for (pool_index = 0; pool_index < 0x60; pool_index++, slot++) {
        if (slot->flags & slot_in_use_bit) {
            func_00216C48(&slot->position, transform, &slot->position);
            live_left--;
            if (live_left <= 0) {
                break;
            }
        }
    }

    actor_in_use_bit = EFFECT_FLAG_LIVE;
    live_left = D_004468D0;
    for (pool_index = 0; pool_index < 0x180; pool_index++, effect++) {
        if (effect->flags & actor_in_use_bit) {
            live_left--;
            model_position = &effect->model->position;
            func_00216C48(model_position, transform, model_position);
            func_00216C48(&effect->unk70, transform, &effect->unk70);
            if (live_left <= 0) {
                break;
            }
        }
    }
}
