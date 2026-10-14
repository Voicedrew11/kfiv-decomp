#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "level=": sets PlayerState.level. */
void Ini_SetLevel(char *text) {
    D_00413F80.level = atoi(text);
}
