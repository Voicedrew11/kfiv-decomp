#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "gold=": sets PlayerState.unk4C. */
void func_00207A60(char *text) {
    D_00413F80.unk4C = atoi(text);
}
