#include "kf_entity.h"

int Motion_SeekSlotKeys(ModelInstance *, int, int, int);

/* the slot's words as one: bit 5 is EntityMotionSlot.moves_entity */
#define SLOT_WORD(slot) (*(u32 *)(slot))

int func_0019A448(Entity *entity, EntityMotionSlot *slot, int part_slot, int animation, int frame, int blend_frames,
                  int force) {
    u8 *motions = entity->params->unk60;
    int motion = motions[animation];
    int current = motions[slot->index];
    ModelPart *root;

    if (force == 0 && current == motion) {
        return 1;
    }
    slot->frame_fixed = frame << 8;
    slot->index = animation;
    slot->frame = frame;
    if (entity->unk28 != 0 && (entity->unkC6 & 0x46) == 0) {
        f32 blend = blend_frames;

        slot->unk01 = blend + blend * 0.75f;
    } else {
        slot->unk01 = blend_frames;
    }
    if (SLOT_WORD(slot) & 0x20) {
        root = ENTITY_MODEL(entity)->unk130;
        root->unk40[0] = root->unk40[1] = root->unk40[2] = 0.0f;
    }
    return Motion_SeekSlotKeys(ENTITY_MODEL(entity), part_slot, motion, frame);
}
