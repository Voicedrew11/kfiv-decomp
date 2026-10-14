#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "gold=": sets PlayerState.gold. */
void Ini_SetGold(char *text) {
    D_00413F80.gold = atoi(text);
}
