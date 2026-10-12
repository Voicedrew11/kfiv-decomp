#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_shortcut_type=": 0 to 2 goes to PlayerState.unk48, anything else gives 0. */
void func_00207CF8(char *text) {
    int number = atoi(text);
    PlayerState *player = &D_00413F80;

    if ((u32)number < 3) {
        player->unk48 = number;
    } else {
        player->unk48 = 0;
    }
}
