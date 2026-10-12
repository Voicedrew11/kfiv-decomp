#include "kf_link.h"
#include "kf_load.h"
#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_state.h"

extern f32 D_00425510[0x100];       /* one alpha per menu part */
extern u32 D_00446A30;              /* how many parts the main menu has */
extern s32 D_00446AC8;
extern void *D_00446B20;
extern f32 D_00446C04;
extern LinkNode *D_00446C90;
extern u16 D_00446DE8;
extern SpritePacket *D_0044722C;
extern s8 D_004472C4;               /* 0: fading in, 1: open, 2: fading out to the chosen screen, 3: fading out to leave */
extern s8 D_004472CD;               /* the line the cursor is on, 0 to 6 */
extern s8 D_004472D8;
extern s8 D_004472DA;               /* nonzero while the menu takes input */
extern ChunkUpload D_005CEEE0;

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
void func_001D49D0(void);
void func_001D4E58(void);
void func_001D4FD0(void);
void func_001D8260(int *);
void func_001E4568(void);
int func_001E5210(u8);
int func_001F92D0(void);
int func_001F93A8(void);
void func_00207478(int, int, int, int);

/* The main menu: moves the cursor over its seven lines, draws the parts, and fades out to the chosen screen. */
void func_001D7C90(HandlerContext *context) {
    u32 i;
    s32 line_count;
    u32 full_alpha;
    u8 kind;

    line_count = 7;
    if ((D_00446DE8 & 0x10) && D_004472DA != 0) {
        D_004472CD--;
        func_0019CDC0(1);
    } else if ((D_00446DE8 & 0x40) && D_004472DA != 0) {
        D_004472CD++;
        func_0019CDC0(1);
    } else if ((D_00446DE8 & 0x4000) && D_004472DA != 0) {
        if ((func_001F92D0() == 0 && D_004472CD == 5) || (func_001F93A8() == 0 && D_004472CD == 1)) {
            func_0019CDC0(3);
        } else {
            func_0019CDC0(0);
            D_004472C4 = 2;
            D_004472DA = 0;
            for (i = 0; i < D_00446A30; i++) {
                if (D_0044722C[i].unk3E == 1 && D_004472CD == D_0044722C[i].slot) {
                    D_00425510[i] = D_00446C04;
                }
            }
        }
    } else if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        D_004472DA = 0;
        D_004472C4 = 3;
        func_0019CDC0(2);
        for (i = 0; i < D_00446A30; i++) {
            if (D_0044722C[i].unk3E == 1 && D_004472CD == D_0044722C[i].slot) {
                D_00425510[i] = D_00446C04;
            }
        }
    }
    D_004472CD = (D_004472CD + 7) % line_count;

    for (i = 0; i < D_00446A30; i++) {
        kind = D_0044722C[i].unk3E;
        func_001E5210(D_0044722C[i].unk42);
        if (D_004472C4 != 1) {
            func_00207478((int)&D_0044722C[i], kind + 1000, D_004472CD, (int)D_00425510[i]);
        } else {
            func_00207478((int)&D_0044722C[i], kind, D_004472CD, (int)D_00425510[i]);
        }
        func_001E5210(0);
    }

    switch (D_004472C4) {
    case 0:
        for (i = 0; i < D_00446A30; i++) {
            D_00425510[i] += D_0044722C[i].alpha / 10.0f;
            if (D_00425510[i] > D_0044722C[i].alpha) {
                for (i = 0; i < D_00446A30; i++) {
                    full_alpha = D_0044722C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_004472C4 = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
        for (i = 0; i < D_00446A30; i++) {
            D_00425510[i] -= D_0044722C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                D_00425510[i] = 0.0f;
                if (D_0044722C[i].unk3E != 1 || D_004472CD != D_0044722C[i].slot) {
                    memset(D_00425510, 0, 0x400);
                    if (D_004472C4 == 2) {
                        if (func_001F92D0() == 0 && D_004472CD == 5) {
                            D_004472C4 = 0;
                            break;
                        }
                        if (func_001F93A8() != 0 || D_004472CD != 1) {
                            D_00446B20 = (void *)func_001D8260;
                            context->unk04 = D_004472CD;
                            switch (context->unk04) {
                            case 0:
                                ((void (*)(s32))func_001E4568)(0); // match: the callee takes nothing
                                break;
                            case 1:
                                ((void (*)(s32))func_001D4FD0)(0); // match: the callee takes nothing
                                break;
                            case 2:
                                ((void (*)(s32))func_001D49D0)(0); // match: the callee takes nothing
                                break;
                            case 4:
                                if (D_00446C90 != NULL) {
                                    D_00446C90->next = (LinkNode *)&D_005CEEE0;
                                } else {
                                    D_00446C90 = (LinkNode *)&D_005CEEE0;
                                }
                                D_004472D8 = 0;
                                break;
                            case 5:
                                ((void (*)(s32))func_001D4E58)(0); // match: the callee takes nothing
                                break;
                            }
                        }
                    } else if (D_004472C4 == 3) {
                        context->unk04 = 0;
                        D_004472CD = 0;
                        D_00446AC8++;
                    }
                    D_004472C4 = 0;
                    break;
                }
            }
        }
        break;
    }
}
