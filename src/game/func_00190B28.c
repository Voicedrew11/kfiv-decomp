#include "common.h"
#include "kf_entity.h"

/* Moves *level by delta, keeping it between 0x40 and max.
   match: max is a parameter so the 0xC0 cap stays in a register (slt/movn), as in EntityAi_RaiseAttackWeight */
static inline void adjust_level(u8 *level, s32 delta, s32 max) {
    s32 value = *level + delta;

    if (value < 0x40) {
        value = 0x40;
    }
    if (value > max) {
        value = max;
    }
    *level = value;
}

/* Lowers one of the entity's unkC0 bytes, picked by its flag bits 57-59, kept between 0x40 and 0xC0. */
void func_00190B28(Entity *entity) {
    EntityFlagBits *flags = ENTITY_FLAG_BITS(entity);
    u32 high_bits = entity->flags >> 57;
    s32 index;
    u8 *level;

    if (high_bits & 1) {
        index = flags->bits58 + 3;
    } else {
        index = flags->bits58;
    }
    level = entity->unkC0;
    level += index;
    adjust_level(level, -10, 0xC0);
}
