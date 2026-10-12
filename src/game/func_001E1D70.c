#include "kf_menu.h"
#include "kf_packet.h"

extern u8 D_00349680[0x10];         /* the options block */
extern f32 D_00425510[0x100];       /* one alpha per menu part; [255] the text's */
extern u32 D_00446A68;              /* how many parts the options screen has */
extern s32 D_00446AC8;
extern void *D_00446B20;
extern s32 D_00446BE0;              /* option 0 as it was before it was first toggled */
extern s8 D_00446BE4;               /* 0: option 0 untouched, 1: toggled and not applied yet, 2 to 4: applied */
extern s8 D_00446BE5;               /* counts the two steps of leaving when the screen is a start-up step */
extern f32 D_00446C04;
extern s8 D_00446C08;
extern s8 D_00446C70;               /* 0: fading in, 1: open, 2: fading out */
extern MenuColor D_00446C78;
extern s8 D_00446C94;
extern u16 D_00446DE8;
extern SpritePacket *D_0044725C;
extern s8 D_004472DA;               /* nonzero while the screen takes input */

void *memset(void *, int, u32);
void func_0019C7F0(s32);
void func_0019C8C0(void);
s32 func_0019CDC0(s32);
void func_001CBED8(void);
void func_001CC018(void);
void func_001DF308(MenuScreenContext *);
int func_001E5210(u8);
void func_00207478(int, int, int, int);

/* The options screen: moves the cursor over its seven lines, changes the option under it, draws the parts, and
   fades the screen in and out. */
void func_001E1D70(MenuScreenContext *context) {
    u32 i;
    u8 kind;
    u32 full_alpha;

    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        context->unk04--;
        func_0019CDC0(1);
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        context->unk04++;
        func_0019CDC0(1);
    }
    context->unk04 += 7;
    context->unk04 %= 7;

    switch (context->unk04) {
    case 0:
        if ((D_00446DE8 & 0xA0) && D_004472DA != 0) {
            func_0019CDC0(1);
            if (D_00446BE4 != 1) {
                D_00446BE0 = D_00349680[0];
            }
            D_00446BE4 = 1;
            D_00349680[0]++;
            D_00349680[0] %= 2;
        } else if (D_00446BE4 == 1 && !(D_00446DE8 & 0xA0) && D_004472DA != 0) {
            if (D_00349680[0] == 0) {
                if (D_00446BE0 != 0) {
                    D_00446BE4 = 2;
                    func_001CBED8();
                } else {
                    D_00446BE4 = 4;
                }
            } else {
                if (D_00446BE0 == 0) {
                    D_00446BE4 = 3;
                    func_001CC018();
                } else {
                    D_00446BE4 = 4;
                }
            }
        }
        break;
    case 1:
        if ((D_00446DE8 & 0x20) && D_00349680[1] < 0x7F && D_004472DA != 0) {
            D_00349680[1] += 4;
            func_0019CDC0(1);
        } else if ((D_00446DE8 & 0x80) && D_00349680[1] != 0 && D_004472DA != 0) {
            D_00349680[1] -= 4;
            func_0019CDC0(1);
        }
        func_0019C7F0(D_00349680[1]);
        func_0019C8C0();
        break;
    case 2:
        if ((D_00446DE8 & 0x20) && D_00349680[2] < 0x7F && D_004472DA != 0) {
            D_00349680[2] += 4;
            func_0019CDC0(1);
        } else if ((D_00446DE8 & 0x80) && D_00349680[2] != 0 && D_004472DA != 0) {
            D_00349680[2] -= 4;
            func_0019CDC0(1);
        }
        break;
    case 3:
        if ((D_00446DE8 & 0xA0) && D_004472DA != 0) {
            D_00349680[5]++;
            func_0019CDC0(1);
        }
        D_00349680[5] = (s8)D_00349680[5] % 2;
        break;
    case 4:
        if ((D_00446DE8 & 0xA0) && D_004472DA != 0) {
            D_00349680[6]++;
            func_0019CDC0(1);
        }
        D_00349680[6] = (s8)D_00349680[6] % 2;
        break;
    case 5:
        if ((D_00446DE8 & 0xA0) && D_004472DA != 0) {
            func_0019CDC0(1);
            D_00349680[9]++;
        }
        D_00349680[9] = (s8)D_00349680[9] % 2;
        break;
    case 6:
        if ((D_00446DE8 & 0x20) && D_004472DA != 0) {
            func_0019CDC0(1);
            D_00349680[8]++;
        } else if ((D_00446DE8 & 0x80) && D_004472DA != 0) {
            func_0019CDC0(1);
            D_00349680[8]--;
        }
        D_00349680[8] = (s8)(D_00349680[8] + 3) % 3;
        break;
    }

    for (i = 0; i < D_00446A68; i++) {
        kind = D_0044725C[i].unk3E;
        func_001E5210(D_0044725C[i].unk42);
        func_00207478((int)&D_0044725C[i], kind + 1000, context->unk04, (int)D_00425510[i]);
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472DA = 0;
        D_00446C70 = 2;
    }

    switch (D_00446C70) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A68; i++) {
            D_00425510[i] += D_0044725C[i].alpha / 10.0f;
            if (D_00425510[i] > D_0044725C[i].alpha) {
                for (i = 0; i < D_00446A68; i++) {
                    full_alpha = D_0044725C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_00446C70 = 1;
                break;
            }
        }
        break;
    case 2:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A68; i++) {
            D_00425510[i] -= D_0044725C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                if (D_00446C94 == 2) {
                    D_00446BE5++;
                } else {
                    D_00446BE5 = 2;
                }
                if (D_00446BE5 == 2) {
                    D_00446BE5 = 0;
                    D_00446C70 = 0;
                    context->unk04 = 0;
                    if (D_00446BE4 == 1) {
                        if (D_00349680[0] == 0) {
                            if (D_00446BE0 != 0) {
                                D_00446BE4 = 2;
                                func_001CBED8();
                            } else {
                                D_00446BE4 = 4;
                            }
                        } else {
                            if (D_00446BE0 == 0) {
                                D_00446BE4 = 3;
                                func_001CC018();
                            } else {
                                D_00446BE4 = 4;
                            }
                        }
                    }
                    if (D_00446C94 != 2) {
                        D_00446B20 = (void *)func_001DF308;
                    } else {
                        D_00446AC8++;
                    }
                    D_00446C08 = 0;
                    D_00446C04 = 128.0f;
                }
                break;
            }
        }
        break;
    }
}
