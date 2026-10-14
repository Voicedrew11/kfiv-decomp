#include "kf_map.h"
#include "kf_message.h"
#include "kf_item.h"

extern u8 item_counts[];
extern ItemCommonDef D_0041A6F8[];

void Menu_AddKeyItem(int); // match: one argument is passed (func_002135A0 declares three)
/* Adds one of an item to what the player carries: up to 1 for weapons and worn items and for items with flag
   bit 1, up to 99 for the others ("Can't carry any more" past that). */
void Player_AddItem(int index) {
    ItemCommonDef *def;

    if (index >= ITEM_WORN_FIRST) {
        if (item_counts[index] == 0) {
            item_counts[index] = 1;
        }
    } else if (index >= ITEM_WEAPON_FIRST) {
        if (item_counts[index] == 0) {
            item_counts[index] = 1;
        }
    } else if (index >= 0) {
        def = &D_0041A6F8[index];
        if (def->flags >> 1 & 1) {
            if (item_counts[index] == 0) {
                item_counts[index] = 1;
                if (def->type == 3) {
                    Menu_AddKeyItem(index);
                }
            }
        } else if (item_counts[index] < 99) {
            item_counts[index]++;
            if (def->type == 3) {
                Menu_AddKeyItem(index);
            }
        } else {
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_CANT_CARRY_ANY_MORE);
        }
    }
}
