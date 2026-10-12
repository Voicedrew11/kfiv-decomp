#include "kf_game.h"

extern s64 D_00432E20[];
extern s64 D_00433050[];
extern char D_00446900[];
extern char D_00446908[];
extern char D_00446910[];

void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_001CF4D0(void);
void func_001CFBB0(char *, ...);
void func_001CFC20(char);
void func_0020B780(int);
void func_00217008(void);
void *func_00217760(int, int);

/* Debug editor for a four-digit decimal number on an "INI:" screen: left and right move
   the cursor, up and down change the digit under it. Stores the number on confirm. */
void func_001B1030(int *value) {
    s64 saved_colours[4];
    int digits[4];
    int cursor = 0;
    int i;
    PadReport *report;
    u16 pressed;

    digits[0] = *value % 10;
    digits[1] = *value / 10 % 10;
    digits[2] = *value / 100 % 10;
    digits[3] = *value / 1000 % 10;
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
        if ((pressed & 0x80) && cursor < 3) {
            cursor++;
        } else if ((pressed & 0x20) && cursor > 0) {
            cursor--;
        } else if (pressed & 0x10) {
            digits[cursor] = (digits[cursor] + 1) % 10;
        } else if (pressed & 0x40) {
            digits[cursor] = (digits[cursor] + 9) % 10;
        }
        func_001095E0();
        func_001095D0();
        func_001CFC20(9);
        func_001CFBB0(D_00446900);
        for (i = 3; i >= 0; i--) {
            func_001CFC20((cursor == i) ? 8 : 9);
            func_001CFBB0(D_00446908, digits[i]);
        }
        func_001CFBB0(D_00446910);
        func_001CF4D0();
        func_001095D8();
        func_001096E8();
        ((int (*)(void))func_00109800)(); // match: a result keeps $2 busy after the call
        if (pressed & 0x2000) {
            *value = digits[0] + digits[1] * 10 + digits[2] * 100 + digits[3] * 1000;
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
}
