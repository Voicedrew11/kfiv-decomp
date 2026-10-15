#include "common.h"

extern u8 D_00413F80[];
extern u8 weapon_defs[];

typedef struct {
    char pad0[0x82];
    u16 f82;
} Ctl;

typedef struct {
    char pad0[0xFC];
    Ctl *fFC;
} Dev;

extern Dev *D_00446978;

void func_00109098(Dev *, u8, u16);
void Sound_StopVoice(s8);
int Sound_PlaySw(int); // match: this file only matches with this declaration

void func_001BBFC8(void) {
    if (D_00413F80[0x261] == 0 && weapon_defs[D_00413F80[0x3C] * 0x90 + 0x63] != 2) {
        Sound_StopVoice(2);
        Sound_PlaySw(1);
        D_00413F80[0x261] = 1;
        func_00109098(D_00446978, 0, D_00446978->fFC->f82);
    }
}
