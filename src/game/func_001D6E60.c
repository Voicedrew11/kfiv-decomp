#include "kf_link.h"
#include "kf_menu.h"
#include <eeregs.h>

extern LinkNode D_00426470;
extern MenuColor D_00446C78;
extern f32 D_0042B7D0[16];

u32 *func_001FBE70(void);
void func_00201E10(LinkNode *, f32 *, MenuColor); // match: the colour is loaded unaligned, so not the definition's types
void sceVu0ViewScreenMatrix(void *, float, float, float, float, float, float, float, float, float);

/* Draws the menu's item model with the matrix given: sends a three-quadword packet of GS settings,
   draws the model of node D_00426470 and puts the view-to-screen matrix back. */
int func_001D6E60(f32 *matrix) { // match: callers declare an int result; nothing is returned
    u32 *packet;

    packet = func_001FBE70();
    packet[0] = 0x8002;
    packet[1] = 0x10000000;
    packet[2] = 0xE;
    packet[3] = 0;
    packet[4] = 0x60F0;
    packet[5] = 0x7990;
    packet[6] = 0x19;
    packet[7] = 0;
    packet[8] = 0x44;
    packet[9] = 0;
    packet[10] = 0x43;
    packet[11] = 0;
    while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
    *D2_QWC = 3;
    *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
    *D2_CHCR = 0x100;
    func_00201E10(&D_00426470, matrix, D_00446C78);
    sceVu0ViewScreenMatrix(D_0042B7D0, 512.0f, 1.0f, 1.0f, 2048.0f, 2048.0f, 0.0f, 16777215.0f, 1.0f, 65536.0f);
}
