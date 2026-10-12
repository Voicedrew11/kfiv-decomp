#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_weapon=": the weapon in hand is item 106 plus the number, or none when it is negative. */
void func_00207A88(char *text) {
    int number = atoi(text);
    PlayerState *player = &D_00413F80;

    if (number >= 0) {
        player->unk3C = number + 106;
    } else {
        player->unk3C = 0;
    }
}
