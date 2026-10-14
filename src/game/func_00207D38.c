#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "equip_shortcut=": sets PlayerState.shortcut, 0 when the number is negative. */
void Ini_SetShortcut(char *text) {
    int number = atoi(text);

    if (number < 0) {
        number = 0;
    }
    D_00413F80.shortcut = number;
}
