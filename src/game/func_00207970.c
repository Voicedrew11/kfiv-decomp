#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "hp=": sets PlayerState.hp_base. */
void Ini_SetHp(char *text) {
    D_00413F80.hp_base = atoi(text);
}
