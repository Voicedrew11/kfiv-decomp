#include "kf_gs.h"
#include "kf_splash.h"

extern int D_00447358;
extern int D_00447364;
extern u32 *D_00446E84;

void func_0010A4E8(int, int, int, int, int);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00109E88(void);
void func_0010A100(void *);

/* Draw the splash image while it is on screen (states 3..5). */
void func_00208540(void) {
    u32 *packet;
    u32 *cursor;

    if (D_00447358 - 3U < 3) {
        func_0010A4E8(0, 0, 0, D_00447364 / 2, 0x44);
        func_00109A50();
        func_00109A80(GS_REG_TEST_1, 0x3000D);
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109A80(GS_REG_ALPHA_1, 0x44);
        func_00109A80(8, 5);
        func_00109AD0();

        packet = cursor = func_00109990(2);
        *cursor++ = *D_00446E84 | 0x30000000;
        *cursor++ = (u32)(D_00446E84 + 4);
        *cursor++ = 0;
        *cursor++ = 0;
        func_00109650(packet, cursor);
        func_00109E88();

        D_005F9B00.alpha = D_00447364;
        func_0010A100(&D_005F9B00);
        func_00109A50();
        func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
        func_00109A80(8, 0);
        func_00109AD0();
    }
}
