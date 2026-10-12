#include "kf_menu.h"
#include "kf_packet.h"
#include "kf_state.h"

extern void (*const D_004402F8[])(HandlerContext *);
extern f32 D_00425510[0x100];
extern MenuItem *D_00426110[4];
extern MenuItem *D_005CF380[4];
extern u32 D_00446A74;
extern SpritePacket *D_00446BFC;
extern MenuItem *D_00446C1C;
extern MenuItem *D_00446C20;
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C34;
extern s32 D_00446C38;
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern int D_00446C48;
extern void (*D_00446CA0)(HandlerContext *);
extern int D_00446CA4;
extern u16 D_00446DE8;
extern s8 D_00447311;
extern s8 D_00447318;

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
void func_001E4568(void);
int func_001E5210(u8);
void func_001E6928(void);
f32 func_001E6A20(s32, s32, f32);
void func_001FC5E8(MenuCursorBar *);
void func_00207478(int, int, int, int);

/* match: the part number is passed sign-extended and no result is kept (the definition takes a u8 and returns int) */
#define SELECT_PART(part) ((void (*)(s32))func_001E5210)(part)

/* The first trade screen, where one of two rows is picked (pad 0x10 / 0x40): draws the screen's parts with the
   cursor bar on the chosen row, fades them in and out, and hands over to the screen of the row taken. */
void func_001E6AB8(HandlerContext *context) {
    MenuCursorBar bar;
    u32 i;
    u32 full_alpha;
    s32 bar_drawn;
    s32 kind;

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x10) {
            func_0019CDC0(1);
            context->unk04--;
        } else if (D_00446DE8 & 0x40) {
            func_0019CDC0(1);
            context->unk04++;
        }
    }
    context->unk04 = (context->unk04 + 2) % 2;

    bar_drawn = 0;
    for (i = 0; i < D_00446A74; i++) {
        if (bar_drawn == 0 && D_00446BFC[i].unk43 == 1) {
            bar.first.f.lo16 = 0x76C0;
            bar.first.f.mid16 = context->unk04 * 0xE0 + 0x7F50;
            bar.first.f.mid24 = 900000;
            bar.second.f.lo16 = bar.first.f.lo16 + 0x12C0;
            bar.second.f.mid16 = bar.first.f.mid16 + 0xD0;
            bar.second.f.mid24 = bar.first.f.mid24;
            bar.red = 0xFF;
            bar.green = 0xFF;
            bar.blue = 0x78;
            bar_drawn = 1;
            if (D_00447311 == 1) {
                D_00425510[0xFD] = func_001E6A20(0x10, 0x20, D_00425510[0xFD]);
            } else {
                D_00425510[0xFD] = 0.0f;
            }
            bar.alpha = (s32)D_00425510[0xFD];
            func_001FC5E8(&bar);
        }
        kind = D_00446BFC[i].unk3E;
        SELECT_PART((s8)D_00446BFC[i].unk42);
        func_00207478((int)&D_00446BFC[i], kind + 1000, 1, (int)D_00425510[i]);
        SELECT_PART(0);
    }

    if (D_00447318 == 0) {
        if (D_00446DE8 & 0x4000) {
            if (context->unk04 == 0) {
                if (D_005CF380[0] != NULL) {
                    func_0019CDC0(0);
                    D_00447311 = 2;
                    D_00447318 = 1;
                } else {
                    func_0019CDC0(3);
                }
            } else if (context->unk04 == 1) {
                if (D_00426110[0] != NULL) {
                    func_0019CDC0(0);
                    D_00447318 = 1;
                    D_00447311 = 3;
                } else {
                    func_0019CDC0(3);
                }
            }
        } else if (D_00446DE8 & 0x2000) {
            func_0019CDC0(2);
            D_00447311 = 4;
            D_00447318 = 1;
        }
    }

    switch (D_00447311) {
    case 0:
        D_00425510[0xFD] = 16.0f;
        for (i = 0; i < D_00446A74; i++) {
            D_00425510[i] += (f32)D_00446BFC[i].alpha / 10.0f;
            if ((f32)D_00446BFC[i].alpha < D_00425510[i]) {
                for (i = 0; i < D_00446A74; i++) {
                    full_alpha = D_00446BFC[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_00447318 = 0;
                D_00447311 = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
    case 4:
        D_00425510[0xFD] -= 1.6f;
        if (D_00425510[0xFD] < 0.0f) {
            D_00425510[0xFD] = 0.0f;
        }
        for (i = 0; i < D_00446A74; i++) {
            D_00425510[i] -= (f32)D_00446BFC[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                D_00446CA0 = D_004402F8[context->unk04];
                if (D_00447311 == 2) {
                    D_00446C40 = D_00446C24;
                    D_00446C44 = D_00446C28;
                    D_00446C48 = D_00446C38;
                    ((void (*)(s32))func_001E6928)(0); // match: the callee takes nothing
                } else if (D_00447311 == 3) {
                    D_00446C40 = D_00446C1C;
                    D_00446C44 = D_00446C20;
                    D_00446C48 = D_00446C34;
                    ((void (*)(s32))func_001E4568)(0); // match: the callee takes nothing
                } else {
                    D_00446CA4++;
                }
                D_00447311 = 0;
                context->unk04 = 0;
                break;
            }
        }
        break;
    }
}
