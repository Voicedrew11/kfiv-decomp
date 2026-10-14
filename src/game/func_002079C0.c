#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "power=": sets PlayerState.power_base. */
void Ini_SetPower(char *text) {
    D_00413F80.power_base = atoi(text);
}
