#include "kf_game.h"
#include "kf_player.h"

extern PlayerState D_00413F80;
extern PlayerItemDef D_0041B9B8[];
extern GameStatus D_0038CC50;
extern void (*D_00413F50[])(u16, u16);
extern u16 D_0044693E;

void func_001A3918(int);
int func_001BC9E0(int);
void func_001BD208(int);

/* The player's actions for this frame: runs the handler of the item in hand, then the magic button. */
void func_001B6FA0(u16 held, u16 pressed) {
    PlayerItemDef *def;

    D_00413F80.unk259 = 0;
    def = &D_0041B9B8[D_00413F80.unk3C];
    D_00413F80.unk25B = 0;
    if (D_00413F80.unk258 != 0) {
        if (D_0038CC50.unk00 == 0 || (D_0038CC50.unk30 & 1) || D_00413F80.unk1A8 != 0 ||
            (u16)D_00413F80.timed_flags[1] != 0) {
            D_00413F80.unk263 = 1;
            func_001A3918(0x5D);
        }
    }
    D_00413F50[def->unk63](held, pressed);
    if (D_00413F80.unk258 == 0 || (D_00413F80.unk25A == 0 && D_00413F80.unk25B == 0)) {
        if (D_0044693E & pressed) {
            func_001BC9E0(D_00413F80.unk3D);
        }
        if (D_0044693E & held) {
            func_001BD208(D_00413F80.unk3D);
        }
    }
    if (D_00413F80.unk258 != 0) {
        D_00413F80.unk254 = 0;
    } else {
        D_00413F80.unk254++;
    }
}
