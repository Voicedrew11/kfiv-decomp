#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_amulet=": the number is the item; the slot holds it less ITEM_WORN_FIRST (adding
   0x100 - ITEM_WORN_FIRST to a byte), or the group's "None" when it is negative. */
void Ini_SetEquipAmulet(char *text) {
    int number = atoi(text);
    PlayerState *player = &D_00413F80;

    if (number >= 0) {
        player->equipment[EQUIP_AMULET] = number + (0x100 - ITEM_WORN_FIRST);
    } else {
        player->equipment[EQUIP_AMULET] = ITEM_WORN_INDEX(ITEM_AMULET_NONE);
    }
}
