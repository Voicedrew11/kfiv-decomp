#include "kf_game.h"

extern char D_004471E1;
extern MessageSlot *D_004471E4;
extern MessageSlot *D_004471E8;

void func_001CFC80(void) {
    D_004471E1 = 0;
    D_004471E8 = D_004471E4;
    D_004471E4->unk04 = 0;
}
