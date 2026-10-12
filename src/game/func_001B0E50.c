#include "kf_game.h"

extern s64 D_00432E20[];
extern s64 D_00433050[];
extern char D_0043F1D8[];

void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_001CF4D0(void);
void func_001CFBB0(char *, ...);
void func_0020B780(int);
void func_00217008(void);
void *func_00217760(int, int);

/* Debug map picker: left and right (or L2 and R2 held) step *map_id through 0..254 on a
   "MAP nnn" screen. Returns TRUE if the choice was confirmed, FALSE if cancelled. */
int func_001B0E50(int *map_id) {
    s64 saved_colours[4];
    int confirmed = FALSE;
    int map = *map_id;
    PadReport *report;
    u16 pressed;
    u16 held;

    saved_colours[0] = D_00432E20[0];
    saved_colours[1] = D_00432E20[30];
    saved_colours[2] = D_00433050[0];
    saved_colours[3] = D_00433050[30];
    D_00432E20[0] = 0xC000L << 17;
    D_00432E20[30] = 0xC000L << 17;
    D_00433050[0] = 0xC000L << 17;
    D_00433050[30] = 0xC000L << 17;
    do {
        func_0020B780(0);
        func_00217008();
        report = func_00217760(0, 0);
        pressed = report->pressed;
        held = report->held;
        if (pressed & 0x80) {
            map--;
            if (map < 0) {
                map = 0xFE;
            }
        } else if (pressed & 0x20) {
            map++;
            if (map > 0xFE) {
                map = 0;
            }
        } else if (held & 0x400) {
            map--;
            if (map < 0) {
                map = 0xFE;
            }
        } else if (held & 0x800) {
            map++;
            if (map > 0xFE) {
                map = 0;
            }
        }
        func_001095E0();
        func_001095D0();
        func_001CFBB0(D_0043F1D8, map);
        func_001CF4D0();
        func_001095D8();
        func_001096E8();
        ((int (*)(void))func_00109800)(); // match: a result keeps $2 busy after the call
        if (pressed & 0x2000) {
            *map_id = map;
            confirmed = TRUE;
            break;
        }
    } while (!(pressed & 0x4000));
    do {
        func_0020B780(0);
        func_00217008();
        report = func_00217760(0, 0);
    } while (report->held != 0);
    D_00432E20[0] = saved_colours[0];
    D_00432E20[30] = saved_colours[1];
    D_00433050[0] = saved_colours[2];
    D_00433050[30] = saved_colours[3];
    return confirmed;
}
