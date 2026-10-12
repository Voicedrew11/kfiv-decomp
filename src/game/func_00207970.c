#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "hp=": sets PlayerState.unk20. */
void func_00207970(char *text) {
    D_00413F80.unk20 = atoi(text);
}
