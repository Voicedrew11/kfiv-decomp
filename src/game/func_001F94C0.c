#include "kf_file.h"
#include "kf_mc.h"
#include "kf_state.h"

extern ModeState D_005CF3F0[];
extern char D_005CF430[];
extern McIconSys D_005DD020[];
extern char D_004408E0[];
extern s32 D_0044733C;
extern char *D_00447334;

s32 func_0021B0C0(void);
void *func_001FBD90(int);
int func_00102258(char *, int, int);
void func_001F9610(SlotRecord *);
void func_001FBAC8(void);
void *memset(void *, s32, u32);

s32 func_001F94C0(void) {
    SlotRecord *slot;
    s32 file_handle;
    s32 base;
    s32 addr;

    if (func_0021B0C0() > 0) {
        return -1;
    }
    D_005CF3F0->unk1B = 0;
    D_005CF3F0->step = 0;
    D_005CF3F0->retries = 0;
    D_005CF3F0->state = 1;
    D_005CF3F0->unk18 = 1;
    base = (s32)func_001FBD90(0x800);
    addr = (base & 0x3F) == 0 ? base : base + (base & 0x3F);
    D_0044733C = addr;
    addr += 0x480;
    if ((addr & 0x3F) != 0) {
        addr += addr & 0x3F;
    }
    D_00447334 = (char *)addr;
    D_00447338 = D_005DD020;
    file_handle = func_00102258(D_004408E0, 0, 1);
    func_001025C0(file_handle, D_005CF430, 0xDBF0);
    func_001024B0(file_handle);
    D_00428B80[0].unk0C = D_005CF430;
    D_00428B80[0].unk10 = D_005CF430;
    D_00428B80[0].unk14 = D_005CF430;
    D_00428B80[1].unk0C = D_005CF430;
    D_00428B80[1].unk10 = D_005CF430;
    D_00428B80[1].unk14 = D_005CF430;
    memset(D_00447334, 0, 0x2C0);
    memset(D_00447338, 0, 0x3C4);
    memset((void *)D_0044733C, 0, 0x480);
    slot = D_00428B80;
    do {
        func_001F9610(slot);
        slot++;
    } while ((s32)slot < (s32)(D_00428B80 + 2));
    func_001FBAC8();
    return 0;
}
