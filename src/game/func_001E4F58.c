#include "kf_menu.h"

extern MenuScreenState D_005CF2F0;
extern void *D_00446B20;
void func_001E02F8(void);
s32 SaveCard_Open(void);
void SaveCard_ClearLoadFailed(s32);
s32 SaveCard_StartCheck(s8);

/* Resets the menu screen state and makes func_001E02F8 the current handler. */
void func_001E4F58(void) {
    D_005CF2F0.unk16 = 0;
    D_005CF2F0.unk10 = 0;
    D_005CF2F0.unk11 = 0;
    D_005CF2F0.unk14 = 0;
    D_005CF2F0.unk0C = 0;
    D_005CF2F0.unk0D = 0;
    D_005CF2F0.unk18 = 1;
    D_005CF2F0.unk1B = 0;
    D_005CF2F0.unk1E = 0;
    D_005CF2F0.unk1F = 0;
    D_005CF2F0.unk20 = 0;
    D_005CF2F0.unk00 = 0;
    D_005CF2F0.unk0E = 1;
    D_005CF2F0.unk12 = -1;
    SaveCard_Open();
    SaveCard_ClearLoadFailed(0);
    SaveCard_ClearLoadFailed(1);
    SaveCard_StartCheck(1);
    D_00446B20 = (void *)func_001E02F8;
}
