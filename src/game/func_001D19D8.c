#include "kf_gs.h"
#include "kf_packet.h"
#include "kf_player.h"
#include "kf_texture.h"

void func_0010B238(TextureTableHeader *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);

extern char D_00349680[0x10]; // match: read as a signed byte here
extern s32 D_0038A218[];
extern PlayerState D_00413F80;
extern char *D_00446F6C;
extern ScreenSprite D_005CDE18[10];

/* Draws the HUD: the frame, the two bars that fill from the left, the one that fills from the bottom, and the
   rest, each a copy of one of the sprites func_001D1450 set up. */
void func_001D19D8(void) {
    ScreenSprite sprite;
    s32 fade;

    if (D_00349680[9] != 0 && D_0038A218[0] != 0) {
        func_0010B238((TextureTableHeader *)D_00446F6C);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(GS_REG_TEST_1, 0x31001);
        func_00109A80(8, 5);
        func_00109AD0();
        sprite = D_005CDE18[0];
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(8, 0);
        func_00109AD0();
        sprite = D_005CDE18[1];
        sprite.vertex[1].x = sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * D_00413F80.unkBC / 5000;
        sprite.vertex[1].tex_s =
            sprite.vertex[0].tex_s + (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * D_00413F80.unkBC / 5000;
        func_0010A100(&sprite);
        sprite = D_005CDE18[2];
        sprite.vertex[1].x = sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * D_00413F80.unkC0 / 5000;
        sprite.vertex[1].tex_s =
            sprite.vertex[0].tex_s + (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * D_00413F80.unkC0 / 5000;
        func_0010A100(&sprite);
        sprite = D_005CDE18[3];
        sprite.vertex[0].y = sprite.vertex[1].y - (sprite.vertex[1].y - sprite.vertex[0].y) * D_00413F80.unk1A4 / 49;
        sprite.vertex[0].tex_t =
            sprite.vertex[1].tex_t - (sprite.vertex[1].tex_t - sprite.vertex[0].tex_t) * D_00413F80.unk1A4 / 49;
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        sprite = D_005CDE18[4];
        func_0010A100(&sprite);
        sprite = D_005CDE18[5];
        sprite.prim = 0x146;
        sprite.rgba[3] = 0;
        sprite.rgba[2] = 0;
        sprite.rgba[1] = 0;
        sprite.rgba[0] = 0;
        func_0010A100(&sprite);
        if (D_00413F80.unk5C > 0) {
            fade = D_00413F80.unk1A6 * 255 / D_00413F80.unk5C;
        } else {
            fade = 0;
        }
        func_00109A50();
        func_00109A80(GS_REG_TEST_1, ((s64)fade << 4) | 0x30005);
        func_00109A80(0x4A, 1);
        func_00109AD0();
        sprite = D_005CDE18[5];
        sprite.prim = 0x156;
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(GS_REG_TEST_1, 0x3D001);
        func_00109A80(0x4A, 0);
        func_00109AD0();
        sprite = D_005CDE18[6];
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(GS_REG_TEST_1, 0x31001);
        func_00109AD0();
        sprite = D_005CDE18[7];
        if (D_00413F80.unkD0 != 0) {
            sprite.vertex[1].x =
                sprite.vertex[0].x + (sprite.vertex[1].x - sprite.vertex[0].x) * D_00413F80.unkCC / D_00413F80.unkD0;
            sprite.vertex[1].tex_s = sprite.vertex[0].tex_s +
                (sprite.vertex[1].tex_s - sprite.vertex[0].tex_s) * D_00413F80.unkCC / D_00413F80.unkD0;
            func_0010A100(&sprite);
        }
        sprite = D_005CDE18[8];
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_1, 0x48);
        func_00109AD0();
        sprite = D_005CDE18[9];
        func_0010A100(&sprite);
    }
}
