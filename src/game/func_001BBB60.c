#include "kf_player.h"
#include "kf_message.h"

extern PlayerState D_00413F80;
extern const PlayerLevelStep D_0043E130[];

/* Adds experience (twice as much while Fortune is worn) and takes every level it now reaches ("Level UP"). */
void Player_AddExperience(int amount) {
    int level;

    if (D_00413F80.equipment[EQUIP_BRACELET] == ITEM_WORN_INDEX(ITEM_FORTUNE)) {
        amount *= 2;
    }
    D_00413F80.experience += amount;
    if (D_00413F80.experience > 999998) {
        D_00413F80.experience = 999999;
    }
    for (level = D_00413F80.level + 1; level < 100; level++) {
        if (D_00413F80.experience >= D_0043E130[level].experience) {
            D_00413F80.level++;
            D_00413F80.hp_base += D_0043E130[level].hp_gain;
            D_00413F80.mp_base += D_0043E130[level].mp_gain;
            D_00413F80.power_base += D_0043E130[level].power_gain;
            D_00413F80.magic_base += D_0043E130[level].magic_gain;
            Message_ShowNotice(0, 0, 0, 0, 0x2D, MSG_LEVEL_UP);
        }
    }
}
