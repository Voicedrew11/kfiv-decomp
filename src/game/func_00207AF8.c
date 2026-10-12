#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_shield=": the shield is item 36 plus the number (of the items from 220), or none when it is negative. */
void func_00207AF8(char *text) {
    int number = atoi(text);
    PlayerState *player = &D_00413F80;

    if (number >= 0) {
        player->equipment[0] = number + 36;
    } else {
        player->equipment[0] = 0;
    }
}
