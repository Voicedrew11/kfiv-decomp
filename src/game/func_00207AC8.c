#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_magic=": sets PlayerState.unk3D, 0 when the number is negative. */
void func_00207AC8(char *text) {
    int number = atoi(text);

    if (number < 0) {
        number = 0;
    }
    D_00413F80.unk3D = number;
}
