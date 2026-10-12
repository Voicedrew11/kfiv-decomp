#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_state.h"

extern u8 D_00369830[];
extern void (*const D_004402F8[])(void);
extern void (*const D_004404D8[])(void);
extern f32 D_00425510[0x100];       /* one alpha per menu part; [253] the cursor bar's */
extern MenuItem *D_00426110[4];
extern u32 D_00446AA0;              /* how many parts the screen has */
extern u32 D_00446AB8;
extern s32 D_00446AD4;
extern MenuItem *D_00446C1C;
extern MenuItem *D_00446C20;
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C34;
extern s32 D_00446C38;
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern s32 D_00446C48;
extern SpritePacket *D_00446C4C;
extern SpritePacket *D_00446C64;
extern void (*D_00446CA0)(void);
extern void (*D_00446CC4)(void);
extern s32 D_00446CC8;
extern s8 D_00446CD0;               /* nonzero once the cursor is on the three lines */
extern u16 D_00446DE8;
extern s8 D_00447313;
extern s8 D_00447316;               /* 0: fading in, 1: open, 2 to 4: fading out to a list, 5: fading out to leave */
extern s8 D_00447318;               /* nonzero while fading out */

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
void func_001E4568(void);
int func_001E5210(u8);
f32 func_001E6A20(s32, s32, f32);
void func_001ED9A8(s32);
void func_001EEE80(void);
void func_001FC5E8(MenuCursorBar *);
void func_00203F28(int, int, int);
void func_00207478(int, int, int, int);

/* The three-line menu in front of the item lists: draws its parts and the cursor bar, opens the list of the chosen
   line, and fades the parts in and out. */
void func_001EEF78(HandlerContext *context) {
    u32 i;
    s32 line_count;
    s32 bar_drawn;
    MenuCursorBar bar;
    u32 full_alpha;
    u8 kind;

    if (D_00447318 == 0 && D_00446CD0 != 0) {
        if (D_00446DE8 & 0x10) {
            func_0019CDC0(1);
            context->unk04--;
        } else if (D_00446DE8 & 0x40) {
            func_0019CDC0(1);
            context->unk04++;
        }
    }
    line_count = 3;
    context->unk04 = (context->unk04 + 3) % line_count;

    bar_drawn = FALSE;
    for (i = 0; i < D_00446AA0; i++) {
        if (!bar_drawn && D_00446C4C[i].unk43 == 1) {
            bar.first.f.lo16 = 0x76C0;
            bar.first.f.mid16 = context->unk04 * 0xD0 + 0x7EF0;
            bar.first.f.mid24 = 0xDBBA0;
            bar.green = bar.red = 0xFF;
            bar.blue = 0x78;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x12C0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xC0;
            bar.second.f.mid24 = bar.first.f.mid24;
            bar_drawn = TRUE;
            if (D_00447316 == 1) {
                D_00425510[253] = func_001E6A20(0x10, 0x20, D_00425510[253]);
            } else {
                D_00425510[253] = 0.0f;
            }
            bar.alpha = (s32)D_00425510[253];
            func_001FC5E8(&bar);
        }
        ((int (*)(s8))func_001E5210)(D_00446C4C[i].unk42); // match: the byte is passed sign-extended here
        func_00207478((int)&D_00446C4C[i], 1000, 0, (int)D_00425510[i]);
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if (D_00446CD0 == 0) {
        for (i = 0; i < D_00446AB8; i++) {
            kind = D_00446C64[i].unk3E;
            func_00203F28((int)&D_00446C64[i], kind, 1);
        }
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            if (D_00446CD0 == 0) {
                func_0019CDC0(0);
                D_00446CD0 = 1;
            } else {
                D_00446AD4 = 0;
                switch (context->unk04) {
                case 0:
                    func_001ED9A8(0);
                    if (D_00426110[0] != NULL) {
                        func_0019CDC0(0);
                        D_00447318 = 1;
                        D_00447316 = 2;
                    } else {
                        func_0019CDC0(3);
                    }
                    break;
                case 1:
                    func_0019CDC0(0);
                    func_001ED9A8(1);
                    if (D_00369830[0x22] != 0) {
                        D_00447318 = 1;
                        D_00447316 = 3;
                    } else {
                        D_00446CD0 = 0;
                    }
                    break;
                case 2:
                    func_001ED9A8(2);
                    if (D_00446C38 > 0) {
                        func_0019CDC0(0);
                        D_00447318 = 1;
                        D_00447316 = 4;
                    } else {
                        func_0019CDC0(3);
                    }
                    break;
                }
            }
        } else if (D_00446DE8 & 0x2000) {
            func_0019CDC0(2);
            if (D_00446CD0 == 0) {
                D_00446CD0 = 1;
            } else {
                D_00447316 = 5;
                D_00447318 = 1;
            }
        }
    }

    switch (D_00447316) {
    case 0:
        D_00425510[253] += 1.6f;
        if (D_00425510[253] > 16.0f) {
            D_00425510[253] = 16.0f;
        }
        for (i = 0; i < D_00446AA0; i++) {
            D_00425510[i] += D_00446C4C[i].alpha / 10.0f;
            if (D_00425510[i] > D_00446C4C[i].alpha) {
                for (i = 0; i < D_00446AA0; i++) {
                    full_alpha = D_00446C4C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00447318 = 0;
                D_00447316 = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        D_00425510[253] -= 1.6f;
        if (D_00425510[253] < 0.0f) {
            D_00425510[253] = 0.0f;
        }
        for (i = 0; i < D_00446AA0; i++) {
            D_00425510[i] -= D_00446C4C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                // match: the table has two entries; line 2 reads the word after it (D_00440300's first pointer)
                D_00446CA0 = D_004402F8[context->unk04];
                if (D_00447316 == 2) {
                    D_00446C40 = D_00446C1C;
                    D_00446CC4 = D_004404D8[context->unk04];
                    D_00446C44 = D_00446C20;
                    D_00446C48 = D_00446C34;
                    ((void (*)(s32))func_001E4568)(0); // match: the callee takes nothing
                } else if (D_00447316 == 3) {
                    D_00446C40 = D_00446C1C;
                    D_00446CC4 = D_004404D8[context->unk04];
                    D_00446C44 = D_00446C20;
                    D_00446C48 = D_00446C34;
                    D_00447313 = 0;
                    ((void (*)(s32))func_001E4568)(0); // match: the callee takes nothing
                } else if (D_00447316 == 4) {
                    D_00446C40 = D_00446C24;
                    D_00446CC4 = D_004404D8[context->unk04];
                    D_00446C44 = D_00446C28;
                    D_00446C48 = D_00446C38;
                    ((void (*)(s32))func_001EEE80)(0); // match: the callee takes nothing
                } else if (D_00447316 == 5) {
                    D_00446CC8++;
                }
                D_00447316 = 0;
                context->unk04 = 0;
                break;
            }
        }
        break;
    }
}
