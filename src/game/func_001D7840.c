#include "kf_load.h"
#include "kf_menu.h"
#include "kf_texture.h"
#include <eeregs.h>

extern u8 D_0034968A[];
extern LoadedBlockRef D_0042A2A0[];
extern s32 D_00446AC8;
extern void *D_00446B20;
extern f32 D_00446C04;
extern s8 D_00446C08;
extern s8 D_00446C09;
extern s8 D_00446C70;
extern MenuColor D_00446C78;
extern MenuColor D_00446C80;
extern MenuColor D_00446C88;
extern s8 D_00446C94;
extern s32 D_00446DEC;
extern s32 D_00446F38;
extern s32 D_00446F58;
extern u8 D_004472CF;
extern s32 D_004472E0;

void func_001095D0(void);
void func_001095D8(void);
void func_001095E0(void);
void func_001096E8(void);
void func_00109800(void);
void func_00109E88(void);
void func_0010B238(TextureTableHeader *);
s32 func_001CFC48(s32);
void func_001D2680(void);
void func_001D55E8(void);
void func_001D7C30(void);
void func_001E1D70(MenuScreenContext *);
void func_001E2558(void);
void func_001E2750(void);
void func_001E37C8(void);
void func_001E4098(void);
void func_001E4F58(void);
int func_001FBDE8(int);
u32 *func_001FBE70(void);
void func_00204180(TextureHeader *, LoadedBlockRef *);

/* Start-up step of a menu screen: loads its textures and blocks, then picks the handler for the screen D_00446C94 names. */
s32 func_001D7840(void) {
    s32 i = 0;
    u32 *packet;
    LoadedBlockRef *block;

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
    func_001FBDE8(D_00446F58);
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
    if (D_00446C94 == 0) {
        D_004472E0 = 0;
        func_001D2680();
        func_001D55E8();
    }
    switch (D_00446C94) {
    case 0:
        D_00446B20 = (void *)func_001D7C30;
        break;
    case 1:
        D_00446B20 = (void *)func_001E4F58;
        break;
    case 2:
        D_00446B20 = (void *)func_001E1D70;
        break;
    case 3:
        func_001E2558();
        D_00446B20 = (void *)func_001E2750;
        break;
    case 4:
        D_00446B20 = (void *)func_001E37C8;
        break;
    case 5:
        D_004472CF = D_0034968A[0];
        D_00446B20 = (void *)func_001E4098;
        D_00446C09 = 0;
        D_00446C70 = 0;
        break;
    }
    D_00446C04 = 128.0f;
    D_00446AC8++;
    D_00446C08 = 0;
    D_00446C78.blue = 0x80;
    D_00446C78.green = 0x80;
    D_00446C78.red = 0x80;
    D_00446C78.alpha = 0;
    D_00446C80.blue = 0x80;
    D_00446C80.green = 0x80;
    D_00446C80.red = 0x80;
    D_00446C80.alpha = 0;
    D_00446C88.blue = 0x80;
    D_00446C88.green = 0x80;
    D_00446C88.red = 0x80;
    D_00446C88.alpha = 0;
    return TRUE;
}
