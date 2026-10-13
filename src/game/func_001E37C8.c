#include "kf_menu.h"

extern MenuScreenState D_005CF2F0;
extern char D_00425510[];
extern void *D_00446B20;
extern s8 D_00446C09;
extern s8 D_00446C70;
void *memset(void *, int, u32);
void func_001E3858(void);
s32 SaveCard_Open(void);
s32 SaveCard_StartCheck(s8);
s32 SaveCard_Update(void);

/* Opens the menu screen func_001E3858 runs, unless SaveCard_Open fails. */
void func_001E37C8(void) {
    if (SaveCard_Open() >= 0) {
        SaveCard_StartCheck(1);
        SaveCard_Update();
        memset(D_00425510, 0, 0x400);
        D_00446B20 = (void *)func_001E3858;
        D_005CF2F0.unk17 = 0;
        D_00446C70 = 0;
        D_00446C09 = 0;
        D_005CF2F0.unk11 = 0;
        D_005CF2F0.unk0C = 0;
        D_005CF2F0.unk1B = 0;
        D_005CF2F0.unk1C = 0;
        D_005CF2F0.unk04 = 0;
        D_005CF2F0.unk19 = 0;
        D_005CF2F0.unk1A = 0;
        D_005CF2F0.unk12 = -1;
    }
}
