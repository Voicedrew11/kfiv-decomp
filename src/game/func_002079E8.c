#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "magic=": sets PlayerState.magic_base. */
void Ini_SetMagic(char *text) {
    D_00413F80.magic_base = atoi(text);
}
