#include "kf_menu.h"
#include "kf_packet.h"

extern u8 D_00349680[0x10];
extern f32 D_00425510[0x100];
extern u32 D_00446AB0;
extern s32 D_00446AC8;
extern void *D_00446B20;
extern f32 D_00446C04;
extern s8 D_00446C08;
extern SpritePacket *D_00446C5C;
extern s8 D_00446C70;               /* 0: fading in, 1: open, 2: leaving, 3: leaving with the old value put back */
extern s8 D_00446C94;
extern u16 D_00446DE8;
extern f32 D_00447228;
extern u8 D_004472CF;
extern s8 D_004472DA;

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
void func_001DF308(MenuScreenContext *);
int func_001E5210(u8);
void func_00207478(int, int, int, int);

/* The slider screen: moves D_00349680[10] between 0x20 and 0xFC in steps of 4, draws the parts with the slider at
   its place, and fades the screen in and out. */
void func_001E4098(void) {
    u32 i;
    u8 kind;
    u32 full_alpha;

    if ((D_00446DE8 & 0x20) && D_004472DA != 0) {
        func_0019CDC0(1);
        if (D_00349680[10] < 0xFC) {
            D_00349680[10] += 4;
        } else {
            D_00349680[10] = 0x20;
        }
    } else if ((D_00446DE8 & 0x80) && D_004472DA != 0) {
        func_0019CDC0(1);
        if (D_00349680[10] > 0x20) {
            D_00349680[10] -= 4;
        } else {
            D_00349680[10] = 0xFC;
        }
    } else if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        func_0019CDC0(0);
        D_004472DA = 0;
        D_00446C70 = 2;
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        func_0019CDC0(2);
        D_00349680[10] = D_004472CF;
        D_00446C70 = 3;
        D_004472DA = 0;
    }
    D_00447228 = (D_00349680[10] - 32.0f) * 402.0f / 220.0f;
    for (i = 0; i < D_00446AB0; i++) {
        kind = D_00446C5C[i].unk3E;
        func_001E5210(D_00446C5C[i].unk42);
        func_00207478((int)&D_00446C5C[i], kind + 1000, (int)D_00447228 + 0x72C, (int)D_00425510[i]);
        func_001E5210(0);
    }
    switch (D_00446C70) {
    case 0:
        for (i = 0; i < D_00446AB0; i++) {
            D_00425510[i] += D_00446C5C[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C5C[i].alpha) {
                for (i = 0; i < D_00446AB0; i++) {
                    full_alpha = D_00446C5C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00446C70 = 1;
                D_004472DA = 1;
                return;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < D_00446AB0; i++) {
            D_00425510[i] -= D_00446C5C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446C04 = 128.0f;
                D_00446C70 = 0;
                D_00446C08 = 0;
                if (D_00446C94 != 5) {
                    D_00446B20 = (void *)func_001DF308;
                } else {
                    D_00446AC8++;
                }
                return;
            }
        }
        break;
    }
}
