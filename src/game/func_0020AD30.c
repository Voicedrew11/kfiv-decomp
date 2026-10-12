#include "kf_game.h"

#define PAD_BUTTON_START 0x0008

extern int D_00446E90;          /* set once START was pressed during the intro movie */

void func_00217008(void);
void *func_00217760(int, int);

/* The intro movie's per-frame callback (given to func_0021B540 by func_0020AD78):
   polls the controller and returns 1, ending the movie, when START is pressed. */
int D_0020AD30(void) { // match: func_0020AD78 declares it void; it is passed as an int (*)(void)
    func_00217008();
    if (((PadReport *)func_00217760(0, 0))->pressed & PAD_BUTTON_START) {
        D_00446E90 = TRUE;
        return TRUE;
    }
    return FALSE;
}
