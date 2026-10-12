#include "kf_game.h"

extern u16 D_00446DE8;

int func_002179F8(int, int);
void *func_00217760(int, int);
int func_00206DF8(int);
int func_00206F58(int);
int func_002070B8(int, u16);
int func_002071F0(int, u16);

/* Builds the menu's buttons for this frame in D_00446DE8: what was pressed, the held shoulder buttons, and the
   directions as repeating steps, from the directional buttons and, in analog mode, from the left stick. */
s16 func_00206C40(void) {
    PadReport *report = func_00217760(0, 0);
    int horizontal;
    int vertical;
    int mode;
    u16 stick;

    D_00446DE8 = report->pressed & 0xFF0F;
    D_00446DE8 |= report->held & 0xF00;
    horizontal = func_00206DF8(1);
    vertical = func_00206F58(1);
    if (horizontal > 0) {
        D_00446DE8 |= 0x20;
    }
    if (horizontal < 0) {
        D_00446DE8 |= 0x80;
    }
    if (vertical > 0) {
        D_00446DE8 |= 0x10;
    }
    if (vertical < 0) {
        D_00446DE8 |= 0x40;
    }
    mode = func_002179F8(0, 0);
    if (mode == 1 && ((PadReportAxes *)report)->left_stick != 0) {
        stick = 0;
        if (report->left_stick_x < 0x40) {
            stick |= 0x80;
        }
        if (report->left_stick_x > 0xC0) {
            stick |= 0x20;
        }
        if (report->left_stick_y < 0x40) {
            stick |= 0x10;
        }
        if (report->left_stick_y > 0xC0) {
            stick |= 0x40;
        }
        if (!(report->held & 0xF0)) {
            horizontal = func_002070B8(1, stick);
            vertical = func_002071F0(1, stick);
            if (horizontal > 0) {
                D_00446DE8 |= 0x20;
            }
            if (horizontal < 0) {
                D_00446DE8 |= 0x80;
            }
            if (vertical > 0) {
                D_00446DE8 |= 0x10;
            }
            if (vertical < 0) {
                D_00446DE8 |= 0x40;
            }
        }
    }
    if (mode == 0xFF) {
        D_00446DE8 = 0;
    }
    return D_00446DE8;
}
