#include "kf_menu.h"
#include <eeregs.h>

extern MenuColor D_00446B18[]; // match: the initialiser of the local below ({ 0x10, 0x10, 0x10 }); an array so it is not reached through $gp
extern s8 D_00446C94;
int sceGsSyncPath(int, int);
u32 *func_001FBE70(void);

/* On the first menu screen: draws the backdrop sprite, then a rectangle in the colour D_00446B18 over it. */
void func_001D7460(void) {
    f32 unused[24];
    MenuColor color = D_00446B18[0];
    u32 *packet;

    if (D_00446C94 == 0) {
        packet = func_001FBE70();
        packet[0] = 0x8001;
        packet[1] = 0x740B4000;
        packet[2] = 0xF5252160;
        packet[3] = 0;
        packet[4] = 0x16;
        packet[5] = 0;
        *(u64 *)&packet[6] = 0x66802B240;
        packet[8] = 0x80808080;
        *(f32 *)&packet[9] = 1.0f;
        packet[10] = 0;
        packet[11] = 0;
        packet[12] = 0x79006C00;
        packet[13] = 1;
        *(f32 *)&packet[14] = 0.625f;
        *(f32 *)&packet[15] = 0.4375f;
        packet[16] = 0x87009400;
        packet[17] = 1;
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 5;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
        sceGsSyncPath(0, 0);
        packet = func_001FBE70();
        packet[0] = 0x8001;
        packet[1] = 0x44000000;
        packet[2] = 0x5510;
        packet[3] = 0;
        packet[4] = 0x46;
        packet[5] = 0;
        packet[6] = color.red | (color.green << 8) | (color.blue << 16) | 0x40000000;
        packet[7] = 0;
        packet[8] = 0x79006C00;
        packet[9] = 2;
        packet[10] = 0x87009400;
        packet[11] = 2;
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 3;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
    }
}
