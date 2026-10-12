#include "kf_load.h"
#include "kf_menu.h"
#include "kf_state.h"
#include "kf_texture.h"
#include <eeregs.h>

extern f32 D_00425510[0x100];
extern f32 D_00425910[0x100];
extern f32 D_00425D10[0x100];
extern LoadedBlockRef D_0042A2A0[];
extern MenuColor D_00446C78;
extern MenuColor D_00446C88;
extern s8 D_00446C08;
extern s8 D_00446C09;
extern MenuItem *D_00446C24;
extern MenuItem *D_00446C28;
extern s32 D_00446C38;
extern MenuItem *D_00446C40;
extern MenuItem *D_00446C44;
extern s32 D_00446C48;
extern void (*D_00446CA0)(HandlerContext *);
extern int D_00446CA4;
extern void (*const D_004402F8[])(HandlerContext *);
extern s32 D_00446DEC;
extern s32 D_00446F38;
extern s32 D_00446F68;

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
void func_001E57A0(s32);
void func_001E6928(void);
void func_001E6AB8(HandlerContext *);
int func_001FBDE8(int);
u32 *func_001FBE70(void);
void func_00204180(void *, LoadedBlockRef *);

/* Start-up step of the trade screens: loads the textures and blocks, builds the item lists of table context->arg and
   picks the screen's handler (table 4 goes straight to the handler in D_004402F8 with the five link nodes set up). */
s32 func_001E6668(HandlerContext *context) {
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
    func_001E57A0(context->arg);
    func_001D55E8();
    D_00446C08 = 0;
    D_00446C09 = 0;
    if (context->arg != 4) {
        D_00446CA0 = func_001E6AB8;
    } else {
        D_00446CA0 = D_004402F8[0];
        D_00446C40 = D_00446C24;
        D_00446C44 = D_00446C28;
        D_00446C48 = D_00446C38;
        ((void (*)(s32))func_001E6928)(0); // match: the callee takes nothing
        context->unk04 = 0;
    }
    memset(D_00425510, 0, 0x400);
    memset(D_00425910, 0, 0x400);
    memset(D_00425D10, 0, 0x400);
    D_00446C78.blue = 0x80;
    D_00446C78.green = 0x80;
    D_00446C78.red = 0x80;
    D_00446C78.alpha = 0;
    D_00446C88.blue = 0x80;
    D_00446C88.green = 0x80;
    D_00446C88.red = 0x80;
    D_00446C88.alpha = 0;
    D_00446CA4++;
    return TRUE;
}
