#include "kf_entity.h"

s32 Motion_BlendSlotToPose(ModelInstance *, s32, s32, s16, f32);

/* `weight` is passed through to Motion_BlendSlotToPose as it came */
int func_0019A538(Entity *entity, EntityMotionSlot *slot, int part_slot, f32 weight) {
    u8 count;

    Motion_BlendSlotToPose(ENTITY_MODEL(entity), part_slot, entity->params->unk60[slot->index], slot->frame, weight);
    slot->unk01 = count = slot->unk01 - 1;
    return count;
}
