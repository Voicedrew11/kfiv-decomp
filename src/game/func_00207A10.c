#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "level=": sets PlayerState.unk54. */
void func_00207A10(char *text) {
    D_00413F80.unk54 = atoi(text);
}
