#include "kf_load.h"
#include "kf_menu.h"
#include "kf_state.h"
#include "kf_texture.h"
#include <eeregs.h>

extern f32 D_00425510[0x100];
extern f32 D_00425910[0x100];
extern f32 D_00425D10[0x100];
extern LoadedBlockRef D_0042A2A0[];
extern s32 D_00446DEC;
extern s32 D_00446F38;
extern s32 D_00446F68;
extern int (*D_00446CF8)();
extern s32 D_00446CFC;
extern s8 D_00446C08;
extern f32 D_00446C04;

void *memset(void *, int, u32);
void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_00109E88(void);
void func_0010B238(TextureTableHeader *);
s32 func_001CFC48(s32);
void func_001D55E8(void);
s32 func_001F94C0(void);
s32 func_001F9740(s8);
void func_001F71D8(void);
void func_001FBDE8(s32);
u32 *func_001FBE70(void);
void func_00204180(void *, LoadedBlockRef *);

/* Start-up step of the save screen: loads the menu blocks, sends the first packet and resets the save screen state. */
int func_001F6F28(HandlerContext *context) {
    s32 i = 0;
    u32 *packet;
    LoadedBlockRef *block;
    SaveScreen *screen;
    int (*next_handler)();

    func_001095E0();
    func_001095D0();
    func_0010B238((TextureTableHeader *)func_001CFC48(D_00446F38));
    func_00109E88();
    func_001095D8();
    func_001096E8();
    ((s32 (*)(void))func_00109800)(); // match: called as if it returned a value, which frees $2 for the loop below
    for (; i < D_00446DEC; i++) {
        block = &D_0042A2A0[i];
        func_00204180(block->data, block);
    }
    func_001FBDE8(D_00446F68);
    packet = func_001FBE70();
    packet[0] = 0x8001;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0;
    packet[5] = 0;
    packet[6] = 0x3F;
    packet[7] = 0;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 2;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    func_001D55E8();
    // match: taking the handler first orders its lui before the save screen's
    next_handler = func_001F71D8;
    screen = &D_005CF3C0;
    screen->unk12 = -1;
    screen->unk00 = 0;
    D_00446CF8 = next_handler;
    // match: this store order schedules into retail's unk18, unk11, unk14, ... order
    screen->unk14 = 0;
    screen->unk0C = 0;
    screen->unk0D = 1;
    screen->unk0E = 1;
    screen->unk1B = 0;
    screen->unk1E = 0;
    screen->unk1F = 0;
    screen->unk20 = 0;
    screen->unk18 = 1;
    screen->unk11 = 0;
    func_001F94C0();
    func_001F9740(1);
    memset(D_00425510, 0, 0x400);
    memset(D_00425910, 0, 0x400);
    memset(D_00425D10, 0, 0x400);
    D_00446C08 = 0;
    D_00446C04 = 128.0f;
    D_00446CFC++;
    return TRUE;
}
