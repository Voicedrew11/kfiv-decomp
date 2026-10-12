#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "power=": sets PlayerState.unk28. */
void func_002079C0(char *text) {
    D_00413F80.unk28 = atoi(text);
}
