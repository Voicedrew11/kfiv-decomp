#include "kf_gs.h"
#include "kf_packet.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A338(void *);

/* Cover the screen with a blended flat colour, in ten vertical strips. */
void func_0010A4E8(int red, int green, int blue, int alpha, int blend) {
    ScreenSprite sprite;
    int strips_left;

    strips_left = 9;
    func_00109A50();
    func_00109A80(GS_REG_ALPHA_1, blend);
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109AD0();
    sprite.prim = 0x46;
    sprite.rgba[0] = red;
    sprite.rgba[1] = green;
    sprite.rgba[2] = blue;
    sprite.rgba[3] = alpha;
    sprite.vertex[0].x = 0x6C00;
    sprite.vertex[0].y = 0x7200;
    sprite.vertex[1].x = 0x7000;
    sprite.vertex[1].y = 0x8E00;
    sprite.vertex[0].z = 0;
    sprite.vertex[1].z = 0;
    do {
        strips_left--;
        func_0010A338(&sprite);
        sprite.vertex[0].x += 0x400;
        sprite.vertex[1].x += 0x400;
    } while (strips_left >= 0);
}
