#include "kf_player.h"

extern PlayerState D_00413F80;

int func_0019D0F0(int);

void func_001BC0E0(int with_sound) {
    PlayerState *player = &D_00413F80;

    if (player->unk262 == 0) {
        if (with_sound) {
            func_0019D0F0(0x23);
        }
        player->unk262 = 1;
    }
}
