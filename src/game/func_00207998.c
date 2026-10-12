#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "mp=": sets PlayerState.unk24. */
void func_00207998(char *text) {
    D_00413F80.unk24 = atoi(text);
}
