#include "kf_packet.h"
#include <eeregs.h>

extern u64 D_004261A0[47];             /* the frame register of the first buffer ([0]) and of the second ([46]) */
extern s32 D_00446AC4;
extern s8 D_00446C94;
extern int D_00446F78;

/* On the first menu screen: copies the backdrop from the shown buffer into the drawn one in two half-brightness passes. */
s32 func_001D6FB0(void) {
    f32 unused[28]; // match: 0x70 bytes of locals nothing uses
    u32 texture_base; /* where the shown buffer is as a texture: 0x1180 when it is the second one */
    u32 *packet;

    if (D_00446C94 == 0) {
        packet = func_001FBE70();
        texture_base = 0x1180;
        if (D_00446F78 == 0) {
            texture_base = 0;
        }
        packet[1] = 0x100B4000; // match: the tag word packet[0] is written last
        packet[2] = 0xE;
        packet[3] = 0;
        *(u64 *)&packet[4] = (D_004261A0[0] & ~0x1FFL) | 0x192;
        packet[6] = 0x4C;
        packet[7] = 0;
        packet[8] = 0;
        packet[9] = 0;
        packet[10] = 0x18;
        packet[11] = 0;
        packet[12] = 0x30000;
        packet[13] = 0;
        packet[14] = 0x47;
        packet[15] = 0;
        packet[16] = 0x2800000;
        packet[17] = 0xE00000;
        packet[18] = 0x40;
        packet[19] = 0;
        *(u64 *)&packet[20] = texture_base | 0x28028000 | 0x640000000L; /* TEX0: 1024 by 512 */
        packet[22] = 0x6;
        packet[23] = 0;
        packet[24] = 0x80808080;
        *(f32 *)&packet[25] = 1.0f;
        packet[26] = 0x1;
        packet[27] = 0;
        packet[28] = 0;
        packet[29] = 0;
        packet[30] = 0x2;
        packet[31] = 0;
        packet[32] = 0;
        packet[33] = 0;
        packet[34] = 0x5;
        packet[35] = 0;
        *(f32 *)&packet[36] = 0.625f;
        *(f32 *)&packet[37] = 0.875f;
        packet[38] = 0x2;
        packet[39] = 0;
        packet[40] = 0xE002800;
        packet[41] = 0;
        packet[42] = 0x5;
        packet[43] = 0;
        packet[44] = 0;
        packet[45] = 0;
        packet[46] = 0x3F;
        packet[47] = 0;
        *(u64 *)&packet[48] = D_004261A0[0];
        packet[50] = 0x4C;
        packet[51] = 0;
        *(u64 *)&packet[52] = 0x66802B240;
        packet[54] = 0x6;
        packet[55] = 0;
        packet[56] = 0;
        packet[57] = 0;
        packet[58] = 0x2;
        packet[59] = 0;
        packet[60] = 0;
        packet[61] = 0;
        packet[62] = 0x5;
        packet[63] = 0;
        *(f32 *)&packet[64] = 0.625f;
        *(f32 *)&packet[65] = 0.4375f;
        packet[66] = 0x2;
        packet[67] = 0;
        packet[68] = 0xE002800;
        packet[69] = 0;
        packet[70] = 0x5;
        packet[71] = 0;
        *(u64 *)&packet[72] = D_004261A0[46];
        packet[74] = 0x4C;
        packet[75] = 0;
        packet[76] = 0;
        packet[77] = 0;
        packet[78] = 0x2;
        packet[79] = 0;
        packet[80] = 0;
        packet[81] = 0;
        packet[82] = 0x5;
        packet[83] = 0;
        *(f32 *)&packet[84] = 0.625f;
        *(f32 *)&packet[85] = 0.4375f;
        packet[86] = 0x2;
        packet[87] = 0;
        packet[88] = 0xE002800;
        packet[89] = 0;
        packet[90] = 0x5;
        packet[91] = 0;
        packet[92] = 0x50000;
        packet[93] = 0;
        packet[94] = 0x47;
        packet[95] = 0;
        packet[96] = 0;
        packet[97] = 0;
        packet[98] = 0x3F;
        packet[99] = 0;
        packet[100] = 0x44;
        packet[101] = 0;
        packet[102] = 0x42;
        packet[103] = 0;
        packet[0] = 0x8019;
        while ((*D2_CHCR & 0x100) || (*GIF_STAT & 0xC00));
        *D2_QWC = 26;
        *D2_MADR = ((u32)packet & 0x7FFFFFFF) | 0x80000000;
        *D2_CHCR = 0x100;
    }
    sceGsSyncPath(0, 0);
    D_00446AC4++;
    return 1;
}
