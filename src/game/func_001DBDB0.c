#include "kf_menu.h"
#include "kf_packet.h"

extern f32 D_00425510[0x100];       /* one alpha per menu part; [255] the text's */
extern u32 D_00446A50;              /* how many parts the screen has */
extern void *D_00446B20;
extern MenuColor D_00446C78;
extern u16 D_00446DE8;
extern SpritePacket *D_0044724C;
extern s8 D_004472C9;               /* 0: fading in, 1: open, 2: fading out to func_001DC2A8, 3: fading out to leave */
extern s8 D_004472DA;               /* nonzero while the screen takes input */

void *memset(void *, int, u32);
s32 func_0019CDC0(s32);
void func_001D45D0(void);
void func_001D7C30(void);
void func_001DC2A8(void);
int func_001E5210(u8);
void func_00207478(int, int, int, int);

/* A menu screen that only shows its parts: draws them, fades them in, and fades them out when it is left. */
void func_001DBDB0(void) {
    u32 i;
    u32 full_alpha;

    for (i = 0; i < D_00446A50; i++) {
        func_001E5210(D_0044724C[i].unk42);
        func_00207478((int)&D_0044724C[i], D_0044724C[i].unk3E + 1000, 0, (int)D_00425510[i]);
        ((void (*)(u8))func_001E5210)(0); // match: called as returning nothing, which frees $2 for the loop test
    }

    if ((D_00446DE8 & 0x2000) && D_004472DA != 0) {
        func_0019CDC0(2);
        D_004472DA = 0;
        D_004472C9 = 3;
    }

    switch (D_004472C9) {
    case 0:
        D_00425510[255] += 12.8f;
        if (D_00425510[255] > 128.0f) {
            D_00425510[255] = 128.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A50; i++) {
            D_00425510[i] += D_0044724C[i].alpha / 10.0f;
            if (D_00425510[i] > D_0044724C[i].alpha) {
                for (i = 0; i < D_00446A50; i++) {
                    full_alpha = D_0044724C[i].alpha;
                    D_00425510[i] = full_alpha;
                }
                D_004472DA = D_004472C9 = 1;
                break;
            }
        }
        break;
    case 2:
    case 3:
        D_00425510[255] -= 12.8f;
        if (D_00425510[255] < 0.0f) {
            D_00425510[255] = 0.0f;
        }
        D_00446C78.alpha = (s32)D_00425510[255];
        for (i = 0; i < D_00446A50; i++) {
            D_00425510[i] -= D_0044724C[i].alpha / 10.0f;
            if (D_00425510[i] < 0.0f) {
                memset(D_00425510, 0, 0x400);
                if (D_004472C9 == 3) {
                    D_00446B20 = (void *)func_001D7C30;
                    ((void (*)(s32))func_001D45D0)(0); // match: the callee takes nothing
                } else {
                    D_00446B20 = (void *)func_001DC2A8;
                }
                D_004472C9 = 0;
                break;
            }
        }
        break;
    }
}
