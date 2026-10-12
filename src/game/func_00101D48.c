#include "kf_packet.h"

extern u8 D_00349680[0x10];
extern int D_00446F78;
extern int D_00446F7C;

void *memset(void *, int, u32);
void func_001095E0(void);
void func_0010B000(void);
void func_001095D0(void);
void func_0010A008(void);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A100(void *);
void func_001095D8(void);
void func_001096E8(void);
void func_00109800(void);
void func_0020B780(int);

/* Draws the centred 0x280 x 0x1C0 logo sprite into both frame buffers, dimmed by the divisor. */
void func_00101D48(int divisor) {
    ScreenSprite sprite;
    int brightness;
    int pass;

    brightness = (D_00349680[10] << 7) / divisor;
    if (brightness > 0xFF) {
        brightness = 0xFF;
    }
    memset(&sprite, 0, sizeof(sprite));
    sprite.tex0 = 0x26802B240;
    sprite.tex1 = 0x60;
    sprite.prim = 0x156;
    sprite.rgba[2] = brightness;
    sprite.rgba[1] = brightness;
    sprite.rgba[0] = brightness;
    sprite.rgba[3] = 0x80;
    sprite.vertex[0].z = 0xFFFF;
    sprite.vertex[0].x = 0x6BF8;
    sprite.vertex[1].x = 0x93F8;
    sprite.vertex[1].z = 0xFFFF;
    sprite.vertex[1].s = 0x2800;
    sprite.vertex[0].y = 0x7200;
    sprite.vertex[1].y = 0x8E00;
    sprite.vertex[1].t = 0xE00;
    sprite.vertex[0].s = 0;
    sprite.vertex[0].t = 0;
    for (pass = 0; pass < 2; pass++) {
        func_001095E0();
        func_0010B000();
        func_001095D0();
        D_00446F78 = pass;
        D_00446F7C = pass;
        func_0010A008();
        func_00109A50();
        func_00109A80(0x47, 0x31001);
        func_00109A80(0x42, 0x44);
        func_00109A80(8, 5);
        func_00109AD0();
        func_0010A100(&sprite);
        func_00109A50();
        func_00109A80(8, 0);
        func_00109AD0();
        func_001095D8();
        if (pass == 0) {
            D_00446F78 = 1;
            D_00446F7C = 1;
            func_001096E8();
            func_00109800();
            func_0020B780(0);
        }
    }
}
