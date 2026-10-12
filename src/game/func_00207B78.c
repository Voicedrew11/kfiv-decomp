#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_armor=": item 36 plus the number, or the empty slot 55 when it is negative. */
void func_00207B78(char *text) {
    int number = atoi(text);
    PlayerState *player = &D_00413F80;

    if (number >= 0) {
        player->equipment[2] = number + 36;
    } else {
        player->equipment[2] = 55;
    }
}
