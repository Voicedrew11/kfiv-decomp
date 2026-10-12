#include "kf_game.h"
#include "kf_screen.h"

extern GameStatus D_0038CC50;
extern void (*D_0038D650[])(void);
extern void (*D_0038D670[])(void);
extern ScreenFade D_0038D680;

void func_00101618(void);
void func_001020F8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_0010B000(void);
void func_00118A50(void);
void func_001B5460(void);
void func_00208288(void);
void func_0020B780(int);

/* Runs a pending map change to the end, drawing its own frames meanwhile. */
void func_00118958(void) {
    if (D_0038D680.step != 0) {
        func_00118A50();
        func_001020F8();
        func_00208288();
        do {
            func_001B5460();
            func_0020B780(0);
            func_001096E8();
            func_001095E0();
            func_0010B000();
            D_0038D670[D_0038D680.draw_step]();
            D_0038D650[D_0038D680.step]();
            func_00101618();
            ((u8 (*)(void))func_00109800)(); // match: called as returning a value, so the loop test uses $3
        } while (D_0038D680.step != 0);
        D_0038CC50.bit2 = FALSE;
        D_0038CC50.bit5 = FALSE;
    }
}
