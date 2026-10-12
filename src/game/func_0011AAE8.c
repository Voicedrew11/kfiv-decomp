#include "kf_game.h"
#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_texture.h"

extern u8 D_00349680[0x10];
extern ViewerEntry D_0038D6B0[];
extern ScreenQuad D_0038D770;
extern ScreenQuad D_0038D7B0;
extern ScreenQuad D_0038D7F0;
extern ScreenQuad D_0038D830;
extern ScreenQuad D_0038D870;
extern PlayerState D_00413F80;
extern ViewerCornerTable D_0043E760;
extern ViewerCornerTable D_0043E770;
extern char *D_0044705C;
extern int D_00447064;
extern int D_00447068;
extern int D_0044706C;
extern int D_00447070;

float cosf(float);
float sinf(float);
void func_00107CD8(int);
void func_00109650(u32 *, u32 *);
u32 *func_00109990(int);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_0010A100(void *);
void func_0010B410(TextureTableHeader *, s32);

/* Draws the picked viewer's picture and the player's marker on it: a turned, pulsing arrow where the
   position is known, a pulsing square at a fixed spot otherwise; then the picture arrows when the viewer
   has more than one picture. */
void func_0011AAE8(void) {
    ViewerCornerTable corner_us;
    ViewerCornerTable corner_vs;
    int corner_xs[4];
    int corner_ys[4];
    int x;
    int y;
    ViewerEntry *entry = &D_0038D6B0[D_00447064];
    int found;
    int alpha;
    int i;
    f32 cosine;
    f32 sine;
    u32 *packet_start;
    u32 *packet;

    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109AD0();
    func_0010B410((TextureTableHeader *)D_0044705C, D_00447068);
    func_00109E88();
    func_00109E18();
    func_0010A100(&D_0038D7B0);
    found = ((int (*)(int *, int *))entry->unk04)(&x, &y);
    if (found == 1) {
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109AD0();
        func_0010B410((TextureTableHeader *)D_0044705C, entry->count);
        func_00109E88();
        func_00109E18();
        corner_us = D_0043E760;
        corner_vs = D_0043E770;
        if (D_0044706C < 45) {
            alpha = sinf(D_0044706C * 3.1415927f / 45.0f) * 128.0f;
        } else {
            alpha = 0;
        }
        D_0044706C = (D_0044706C + 1) % 60;
        cosine = cosf(D_00413F80.rotation.y);
        sine = sinf(D_00413F80.rotation.y);
        corner_xs[0] = x + (int)((cosine * -32.0f - sine * -44.0f) * 0.5f);
        corner_ys[0] = y + (int)((sine * -32.0f + cosine * -44.0f) * 0.5f);
        corner_xs[1] = x + (int)((cosine * 32.0f - sine * -44.0f) * 0.5f);
        corner_ys[1] = y + (int)((sine * 32.0f + cosine * -44.0f) * 0.5f);
        corner_xs[2] = x + (int)((cosine * -32.0f - sine * 20.0f) * 0.5f);
        corner_ys[2] = y + (int)((sine * -32.0f + cosine * 20.0f) * 0.5f);
        corner_xs[3] = x + (int)((cosine * 32.0f - sine * 20.0f) * 0.5f);
        corner_ys[3] = y + (int)((sine * 32.0f + cosine * 20.0f) * 0.5f);
        packet_start = packet = func_00109990(0x13);
        *packet++ = 0x10000011;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x6C0FC000;
        *(u64 *)packet = 0xE0AA400000008001L;
        packet += 2;
        *(u64 *)packet = 0x513513513513EEL;
        packet += 2;
        *(u64 *)packet = 0x4007FC0599306BC0L;
        packet += 2;
        *packet++ = 6;
        *packet++ = 0;
        *(u64 *)packet = 0x60;
        packet += 2;
        *packet++ = 0x14;
        *packet++ = 0;
        for (i = 0; i < 4; i++) {
            *packet++ = corner_us.values[i];
            *packet++ = corner_vs.values[i];
            *packet++ = 0;
            *packet++ = 0;
            *packet++ = 0x80;
            *packet++ = 0x80;
            *packet++ = 0x80;
            *packet++ = alpha;
            *packet++ = (corner_xs[i] + 0x6C0) << 4;
            *packet++ = (corner_ys[i] + 0x720) << 4;
            *packet++ = 0xFFFFFFFF;
            *packet++ = 1;
        }
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0;
        *packet++ = 0x15000276;
        func_00109650(packet_start, packet);
    }
    if (found == -1) {
        if (D_00447070 < 45) {
            D_0038D770.alpha = sinf(D_00447070 * 3.1415927f / 45.0f) * 128.0f;
        } else {
            D_0038D770.alpha = 0;
        }
        D_00447070 = (D_00447070 + 1) % 60;
        D_0038D770.first_x = (x + 0x690) << 4;
        D_0038D770.first_y = (y + 0x6F0) << 4;
        D_0038D770.second_x = (x + 0x6F0) << 4;
        D_0038D770.second_y = (y + 0x750) << 4;
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        func_0010A100(&D_0038D770);
    }
    if (entry->count >= 2) {
        func_0010B410((TextureTableHeader *)D_0044705C, entry->count * 2 | 1);
        func_00109E88();
        func_00109E18();
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109AD0();
        func_0010A100(&D_0038D7F0);
        func_0010B410((TextureTableHeader *)D_0044705C, entry->count * 2 + 2);
        func_00109E88();
        func_00109E18();
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        func_0010A100(&D_0038D830);
        func_0010B410((TextureTableHeader *)D_0044705C, entry->count + 1 + D_00447068);
        func_00109E88();
        func_00109E18();
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        func_0010A100(&D_0038D870);
    }
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
    func_00107CD8(D_00349680[10]);
}
