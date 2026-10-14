#include "common.h"
#include "kf_item.h"

extern u8 item_counts[];

int func_001F92D0(void) {
    int i;

    for (i = 0; i < ITEM_COUNT; i++) {
        if (item_counts[i] != 0 && (i < ITEM_KEY_FIRST || i == ITEM_KEY_UNUSED || i > ITEM_KEY_LAST) && i != 0
            && i != ITEM_WEAPON_FIRST && i != ITEM_SHIELD_NONE && i != ITEM_HELM_NONE && i != ITEM_ARMOR_NONE
            && i != ITEM_GAUNTLET_NONE && i != ITEM_BOOT_NONE && i != ITEM_RING_NONE && i != ITEM_BRACELET_NONE
            && i != ITEM_AMULET_NONE && i != 0) {
            return TRUE;
        }
    }
    return FALSE;
}
