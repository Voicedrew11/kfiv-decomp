#include "kf_entity.h"

/* Bumps one of the four unkC0 bytes picked by flag bits 58-59, kept between 0x40 and 0xC0. */
void func_0019A348(Entity *entity) {
    u8 *level = entity->unkC0;
    s32 value;
    s32 limit = 0xC0; // match: the 0xC0 cap stays in a register, as in func_00190A30

    level += ENTITY_FLAG_BITS(entity)->bits58;
    value = *level + 5;
    if (value < 0x40) {
        value = 0x40;
    }
    if (value > limit) {
        value = limit;
    }
    *level = value;
}
