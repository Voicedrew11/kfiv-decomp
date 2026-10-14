#include "kf_map.h"

extern u8 item_counts[];
extern ItemCommonDef D_0041A6F8[];

/* TRUE when the player cannot take one more of the item ("Can't carry any more", func_001C9FB0): a one-of-a-kind
   item already carried, or 99 of another. */
int Player_CannotCarryItem(int idx) {
    ItemCommonDef *entry;

    if (idx < 0) {
        return 0;
    }
    entry = D_0041A6F8 + idx;
    if (entry->flags >> 1 & 1) {
        if (item_counts[idx] != 0) {
            return 1;
        }
    } else {
        if (item_counts[idx] >= 0x63) {
            return 1;
        }
    }
    return 0;
}
