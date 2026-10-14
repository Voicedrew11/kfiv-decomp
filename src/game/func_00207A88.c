#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_weapon=": the number is the item; the weapon in hand is that less ITEM_WEAPON_FIRST
   (adding 0x100 - ITEM_WEAPON_FIRST to a byte), or none when it is negative. */
void Ini_SetEquipWeapon(char *text) {
    int number = atoi(text);
    PlayerState *player = &D_00413F80;

    if (number >= 0) {
        player->weapon = number + (0x100 - ITEM_WEAPON_FIRST);
    } else {
        player->weapon = 0;
    }
}
