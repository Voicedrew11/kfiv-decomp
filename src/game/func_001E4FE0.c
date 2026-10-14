#include "common.h"
#include "kf_item.h"

/* Tells whether value (an equipment slot's number) differs from the "None" of the slot the given step stands for. */
int func_001E4FE0(s32 value, u32 step) {
    switch (step) {
        case 0:
        case 1:
        case 2:
        case 11:
            return value != 0;
        case 3:
            return value != ITEM_WORN_INDEX(ITEM_HELM_NONE);
        case 4:
            return value != ITEM_WORN_INDEX(ITEM_ARMOR_NONE);
        case 5:
            return value != ITEM_WORN_INDEX(ITEM_GAUNTLET_NONE);
        case 6:
            return value != ITEM_WORN_INDEX(ITEM_BOOT_NONE);
        case 7:
            return value != ITEM_WORN_INDEX(ITEM_RING_NONE);
        case 8:
            return value != ITEM_WORN_INDEX(ITEM_BRACELET_NONE);
        case 9:
            return value != ITEM_WORN_INDEX(ITEM_AMULET_NONE);
        default:
            return FALSE;
    }
}
