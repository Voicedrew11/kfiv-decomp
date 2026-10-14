#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "exp=": sets PlayerState.experience. */
void Ini_SetExp(char *text) {
    D_00413F80.experience = atoi(text);
}
