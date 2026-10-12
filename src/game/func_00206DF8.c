#include "kf_game.h"

extern s8 D_00446DC0;               /* TRUE while the held direction is repeating */
extern s8 D_00446DC1;               /* frames since the last step, stops at 11 */
extern s32 D_00446DC4;              /* frames right (positive) or left (negative) has been held */

int func_002179F8(int, int);
void *func_00217760(int, int);

/* Right and left on the directional buttons as steps of a menu: 1 or -1 when the buttons are let go
   more than ten frames after the last step, and held for more than ten frames, one step every time the
   count runs from repeat_start past ten again. Nothing while the controller's mode is not known. */
int func_00206DF8(int repeat_start) {
    int step = 0;

    if (func_002179F8(0, 0) == 0xFF) {
        D_00446DC1 = 0;
        D_00446DC4 = 0;
        D_00446DC0 = 0;
        return 0;
    }
    if (D_00446DC1 > 10) {
        D_00446DC1 = 10;
    }
    D_00446DC1++;
    if (!(((PadReport *)func_00217760(0, 0))->held & 0xA0)) {
        if (D_00446DC4 != 0 && D_00446DC0 == 0 && D_00446DC1 > 10) {
            int backward = D_00446DC4 < 1; // match: the test as a value, or the compare turns round
            int back_step = -1;         // match: -1 loaded before step is set

            step = 1;
            if (backward) {
                step = back_step;
            }
            D_00446DC4 = 0;
            D_00446DC1 = 0;
        } else {
            D_00446DC0 = 0;
            D_00446DC4 = 0;
        }
    }
    if (((PadReport *)func_00217760(0, 0))->held & 0x20) {
        D_00446DC4++;
        if (D_00446DC4 > 10) {
            step = 1;
            D_00446DC4 = repeat_start;
            D_00446DC0 = 1;
            D_00446DC1 = 0;
        }
    } else if (((PadReport *)func_00217760(0, 0))->held & 0x80) {
        D_00446DC4--;
        if (D_00446DC4 < -10) {
            D_00446DC4 = -repeat_start;
            step = -1;
            D_00446DC0 = 1;
            D_00446DC1 = 0;
        }
    }
    return step;
}
