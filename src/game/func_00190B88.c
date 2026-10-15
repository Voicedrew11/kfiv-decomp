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

/* Lowers the chosen unkBC byte by 10 and raises the other by 5, each kept between 0x40 and 0xC0. */
void func_00190B88(Entity *entity, s32 chosen) {
    s32 i;

    for (i = 0; i < 2; i++) {
        if (i == chosen) {
            adjust_level(&entity->unkBC[i], -10, 0xC0);
        } else {
            adjust_level(&entity->unkBC[i], 5, 0xC0);
        }
    }
}
