#include "kf_gs.h"
#include "kf_player.h"
#include "kf_texture.h"

void func_0010B238(TextureTableHeader *);
void func_00109E88(void);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
int func_001C0A80(void);
s32 func_001C0A28(s32);
void func_001C08A0(void *, s32);
extern PlayerState D_00413F80;
extern WeaponTrailKind D_00414660[7];
extern u32 D_00447164;
extern u32 D_0044716C;
extern u32 D_00447174;
extern s32 D_00447188;
extern ModelInstance D_0057B6A0;
extern ModelInstance D_0057B860;

/* draws the held weapon and the hand, then the weapon trail as one triangle strip */
void func_001C0268(void) {
    s32 i;
    s32 count;
    u32 *packet;
    u32 *packet_start;
    u32 clipped;
    s32 size;
    u64 tag;

    if (D_00413F80.unk258 != 0) {
        func_0010B238((TextureTableHeader *)D_00447164);
        func_00109E88();
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(GS_REG_ALPHA_2, 9);
        func_00109A80(GS_REG_TEST_1, 0x5000D);
        func_00109A80(GS_REG_TEST_2, 0x5000D);
        func_00109AD0();
        func_0010B238((TextureTableHeader *)D_00447174);
        func_00109E88();
        D_0057B860.unk1A8(&D_0057B860);
        func_0010B238((TextureTableHeader *)D_0044716C);
        func_00109E88();
        if (D_00413F80.unk3C != 0x2E) {
            D_0057B6A0.unk1A8(&D_0057B6A0);
        } else {
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = i == 5;
            }
            D_0057B6A0.unk1A8(&D_0057B6A0);
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = i != 5;
            }
            func_00109A50();
            func_00109A80(GS_REG_ALPHA_1, 0x48);
            func_00109A80(GS_REG_TEST_1, 0x5000D);
            func_00109AD0();
            D_0057B6A0.unk1A8(&D_0057B6A0);
            for (i = 0; i < D_0057B6A0.part_count; i++) {
                MODEL_PART_FLAG_BITS(&D_0057B6A0.parts[i])->bit40 = FALSE;
            }
        }
    }
    count = func_001C0A80();
    if (count >= 4) {
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, D_00414660[D_00447188].blend);
        func_00109A80(GS_REG_TEST_1, 0x30000);
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109AD0();
        clipped = 6;
        size = count * 2 + 1;
        packet_start = packet = func_00109990(size + 3);
        *packet = (size + 1) | 0x10000000;
        packet += 7;
        *packet++ = size | 0x51000000;
        /* GIF tag: count vertices of two registers each, a shaded, blended triangle strip */
        tag = count | 0x8000 | (0x8000L << 46);
        tag |= 0x9900L << 38; // match: a second statement, or the two constants fold into one
        *(u64 *)packet = tag;
        packet += 2;
        *(u64 *)packet = 0x51; /* the registers: RGBAQ, XYZ2 */
        packet += 2;
        for (i = 0; i < count; i++) {
            clipped >>= 1;
            *packet++ = D_00414660[D_00447188].red;
            *packet++ = D_00414660[D_00447188].green;
            *packet++ = D_00414660[D_00447188].blue;
            *packet++ = D_00414660[D_00447188].alpha * (func_001C0A28(i) - 1) / (D_00414660[D_00447188].lifetime - 1);
            func_001C08A0(packet, i);
            packet += 3;
            /* a point off the screen keeps the three triangles it is part of from being drawn */
            if (*packet != 0) {
                clipped |= 4;
            }
            if (clipped != 0) {
                *packet++ = 0x8000;
            } else {
                *packet++ = 0;
            }
        }
        func_00109650(packet_start, packet);
        func_00109A50();
        func_00109A80(GS_REG_ZBUF_1, 0xA000118);
        func_00109AD0();
    }
}
