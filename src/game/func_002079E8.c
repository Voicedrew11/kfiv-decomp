#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "magic=": sets PlayerState.unk2C. */
void func_002079E8(char *text) {
    D_00413F80.unk2C = atoi(text);
}
