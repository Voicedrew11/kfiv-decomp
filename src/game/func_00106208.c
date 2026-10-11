#include "kf_gs.h"
#include "common.h"

extern int D_00447000[2];
extern int D_00447010;
extern int D_00447014;
extern int D_00468180[];

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A4E8(int, int, int, int, int);

void func_00106208(void) {
    char buf[0x40]; /* unused, but retail reserves the 0x40 bytes */
    int alpha;

    switch (D_00447000[3]) {
    case 0:
        return;
    case 1:
        alpha = (D_00447010 << 7) / D_00447014;
        break;
    case -1:
        alpha = 0x80 - (D_00447010 << 7) / D_00447014;
        break;
    default:
        return;
    }
    if (alpha > 0) {
        func_00109A50();
        func_00109A80(GS_REG_TEST_1, 0x30000);
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109AD0();
        func_0010A4E8(D_00468180[0], D_00468180[1], D_00468180[2], alpha, 0x44);
        func_00109A50();
        func_00109A80(GS_REG_ZBUF_1, 0xA000118);
        func_00109AD0();
    }
}
