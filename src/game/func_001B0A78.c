#include "kf_game.h"

extern DebugStartMenuSpacer D_0043EF70;
extern DebugStartMenuPrompt D_0043EF90;
extern DebugStartMenuItems D_0043EFD8;
extern u16 D_00413DA8[];

void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010A4E8(int, int, int, int, int);
void func_001B5460(void);
void func_001CF4D0(void);
void func_001CFBB0(char *, ...);
void func_001CFC20(char);
void func_0020B780(int);

/* Debug start menu: waits on a PUSH START screen, then lets up and down pick one of four
   lines. Returns the line chosen, or -1. */
int func_001B0A78(void) {
    DebugStartMenuSpacer spacer = D_0043EF70;
    DebugStartMenuPrompt prompt = D_0043EF90;
    DebugStartMenuItems items = D_0043EFD8;
    int chosen = -1;
    int running = TRUE;
    int selected;
    int i;

    do {
        while (running) {
            func_001B5460();
            if (D_00413DA8[1] & 8) {
                break;
            }
            func_001095E0();
            func_001095D0();
            func_0010A4E8(0, 0, 0, 0, 0x2A);
            func_001CFC20(0);
            func_001CFBB0(spacer.text);
            func_001CFBB0(prompt.text);
            func_001CF4D0();
            func_001095D8();
            func_001096E8();
            func_00109800();
            func_0020B780(0);
        }
        selected = 0;
        while (running) {
            func_001B5460();
            if (D_00413DA8[1] & 0x10) {
                selected = (selected + 3) % 4;
            } else if (D_00413DA8[1] & 0x40) {
                selected = (selected + 1) % 4;
            } else if (D_00413DA8[1] & 0x2000) {
                break;
            } else if (D_00413DA8[1] & 0x4000) {
                chosen = selected;
                running = FALSE;
            }
            func_001095E0();
            func_001095D0();
            func_0010A4E8(0, 0, 0, 0, 0x2A);
            func_001CFBB0(spacer.text);
            for (i = 0; i < 4; i++) {
                func_001CFC20((selected == i) ? 3 : 0);
                func_001CFBB0(items.text[i]);
            }
            func_001CF4D0();
            func_001095D8();
            func_001096E8();
            func_00109800();
            func_0020B780(0);
        }
    } while (running);
    return chosen;
}
