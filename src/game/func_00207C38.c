#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_ring=": item 36 plus the number, or the empty slot 130 when it is negative. */
void func_00207C38(char *text) {
    int number = atoi(text);
    PlayerState *player = &D_00413F80;

    if (number >= 0) {
        player->equipment[5] = number + 36;
    } else {
        player->equipment[5] = 130;
    }
}
