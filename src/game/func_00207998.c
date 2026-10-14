#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "mp=": sets PlayerState.mp_base. */
void Ini_SetMp(char *text) {
    D_00413F80.mp_base = atoi(text);
}
