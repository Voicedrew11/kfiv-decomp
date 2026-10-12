#include "common.h"

extern s8 D_00446DD4;               /* TRUE while the held direction is repeating */
extern s8 D_00446DD5;               /* frames since the last step, stops at 11 */
extern s32 D_00446DD8;              /* frames right (positive) or left (negative) has been held */

int func_002179F8(int, int);

/* Right and left in `buttons` as steps of a menu, the way func_00206DF8 does it for the directional buttons:
   1 or -1 when both are let go more than ten frames after the last step, and held for more than ten frames,
   one step every time the count runs from repeat_start past ten again. */
int func_002070B8(int repeat_start, u16 buttons) {
    int step = 0;

    if (func_002179F8(0, 0) == 0xFF) {
        D_00446DD5 = 0;
        D_00446DD8 = 0;
        D_00446DD4 = 0;
        return 0;
    }
    if (D_00446DD5 > 10) {
        D_00446DD5 = 10;
    }
    D_00446DD5++;
    if (!(buttons & 0xA0)) {
        if (D_00446DD8 != 0 && D_00446DD4 == 0 && D_00446DD5 > 10) {
            int backward = D_00446DD8 < 1; // match: the test as a value, or the compare turns round
            int back_step = -1;         // match: -1 loaded before step is set

            step = 1;
            if (backward) {
                step = back_step;
            }
            D_00446DD8 = 0;
            D_00446DD5 = 0;
        } else {
            D_00446DD4 = 0;
            D_00446DD8 = 0;
        }
    }
    if (buttons & 0x20) {
        D_00446DD8++;
        if (D_00446DD8 > 10) {
            step = 1;
            D_00446DD8 = repeat_start;
            D_00446DD4 = 1;
            D_00446DD5 = 0;
        }
    } else if (buttons & 0x80) {
        D_00446DD8--;
        if (D_00446DD8 < -10) {
            D_00446DD8 = -repeat_start;
            step = -1;
            D_00446DD4 = 1;
            D_00446DD5 = 0;
        }
    }
    return step;
}
