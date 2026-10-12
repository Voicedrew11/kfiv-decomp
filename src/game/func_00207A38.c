#include "kf_player.h"

extern PlayerState D_00413F80;

int atoi(const char *);

/* ini/NNNN.ini, key "exp=": sets PlayerState.unk50. */
void func_00207A38(char *text) {
    D_00413F80.unk50 = atoi(text);
}
