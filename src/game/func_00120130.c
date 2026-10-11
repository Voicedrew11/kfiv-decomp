#include "kf_game.h"

void func_0011FDD0(void *, int);
extern GameStatus D_0038CC50;
extern EventRunner D_00420D00;

/* Sets D_0038CC50 bit 1 and makes the event the current one while func_0011FDD0 runs. */
void func_00120130(GameEvent *event, int mode) {
    void *saved;
    int bit;

    if ((event->flags & 0x2FF00) == 0x100 && mode != 0) {
        saved = D_00420D00.current;
        bit = D_0038CC50.bit1;
        D_0038CC50.bit1 = 1;
        D_00420D00.current = event;
        func_0011FDD0(event, mode);
        D_00420D00.current = saved;
        D_0038CC50.bit1 = bit;
    }
}
