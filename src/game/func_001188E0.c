#include "kf_screen.h"

extern ScreenFade D_0038D680;

int func_00103A18(int, int *, int *);

/* Starts the map change that entry (5 and up) of the table func_00103A18 reads leads to. */
int func_001188E0(int entry) {
    int target[2];
    int map;

    if (D_0038D680.step != 0) {
        return FALSE;
    }
    if (func_00103A18(entry - 5, &target[0], &target[1]) == 0) {
        return FALSE;
    }
    map = target[0];
    D_0038D680.step = 1;
    D_0038D680.unk14 = target[1];
    D_0038D680.timer = 30;
    D_0038D680.map = map;
    D_0038D680.kind = 2;
    D_0038D680.draw_step = 0;
    return TRUE;
}
