#include "kf_splash.h"

void *memset(void *, int, u32);

/* Build the splash sprite packet D_005F9B00 for the image at D_00249680. */
void func_00208670(void) {
    int height;
    int width;
    u64 tex0;

    width = 1 << ((D_00249680.tex0 >> 26) & 0xF);
    height = 1 << ((D_00249680.tex0 >> 30) & 0xF);

    memset(&D_005F9B00, 0, sizeof(D_005F9B00));
    tex0 = D_00249680.tex0;
    tex0 &= 0xFFFFFFFFFFFFC000UL;
    tex0 |= 0x2E40;
    tex0 &= 0xFFF8001FFFFFFFFFUL;
    tex0 |= 0xF500UL << 35;
    D_005F9B00.tex0 = tex0;
    D_005F9B00.unk08 = D_00249680.unk18;
    D_005F9B00.gif_reg = 0x156;
    D_005F9B00.red = 0x80;
    D_005F9B00.green = 0x80;
    D_005F9B00.blue = 0x80;
    D_005F9B00.vertex0_x = (0x800 - width / 2) * 16;
    D_005F9B00.vertex0_y = (0x800 - height / 2) * 16;
    D_005F9B00.vertex0_z = 0xFFFFFFFF;
    D_005F9B00.vertex0_s = 0;
    D_005F9B00.vertex0_t = 0;
    D_005F9B00.vertex1_x = (width / 2) * 16 + 0x7FF8;
    D_005F9B00.vertex1_y = (height / 2) * 16 + 0x7FF8;
    D_005F9B00.vertex1_z = 0xFFFFFFFF;
    D_005F9B00.vertex1_s = width * 16;
    D_005F9B00.vertex1_t = height * 16;
}
