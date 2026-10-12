#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_guntlet=": item 36 plus the number, or the empty slot 80 when it is negative. */
void func_00207BB8(char *text) {
    int number = atoi(text);
    PlayerState *player = &D_00413F80;

    if (number >= 0) {
        player->unk41 = number + 36;
    } else {
        player->unk41 = 80;
    }
}
