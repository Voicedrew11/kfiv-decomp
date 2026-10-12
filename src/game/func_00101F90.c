#include "kf_game.h"

extern GameStatus D_0038CC50;
extern u16 D_00413DA8[];
extern u16 D_00446946;
extern PlayTime D_00446A28;

void func_001D20E0(void);
void func_0020B780(int);
void func_001096E8(void);
void func_001095E0(void);
void func_0010B000(void);
void func_00111D58(void);
int func_001005E8(int);
void func_00109800(void);
void func_001B57D8(void);
int sceGsSyncV(int);
void func_001B5460(void);
void func_001B5800(void);
void func_001D2048(PlayTime);

/* Pause: when the pause button is pressed, holds the game until it is pressed again. */
void func_00101F90(void) {
    PlayTime play_time;

    if (D_00413DA8[1] & D_00446946) {
        func_001D20E0();
        D_0038CC50.paused = TRUE;
        play_time = D_00446A28;
        func_0020B780(0);
        func_001096E8();
        func_001095E0();
        func_0010B000();
        func_00111D58();
        func_001005E8(0);
        func_00109800();
        func_0020B780(0);
        func_001096E8();
        func_001095E0();
        func_0010B000();
        func_00111D58();
        func_001005E8(0);
        func_00109800();
        func_0020B780(0);
        func_001B57D8();
        do {
            while (sceGsSyncV(0) != 1) {
            }
            func_001B5460();
        } while (!(D_00413DA8[1] & D_00446946));
        func_001B5800();
        D_0038CC50.paused = FALSE;
        func_001096E8();
        func_001095E0();
        func_0010B000();
        func_00111D58();
        func_001005E8(0);
        func_00109800();
        func_001D2048(play_time);
    }
}
