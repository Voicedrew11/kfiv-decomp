#include "kf_screen.h"
#include "kf_gs.h"

extern ScreenFade D_0038D680;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A4E8(int, int, int, int, int);
void func_00114198(int);

/* Draws one frame of a 30-frame fade and counts the timer down. */
void func_00118CA8(void) {
    func_00114198(0);
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_ALPHA_1, 0x88);
    func_00109AD0();
    {
        int total = 30; /* two separate 30s, as retail keeps both */
        int span = 30;
        func_0010A4E8(0, 0, 0, ((total - D_0038D680.timer) << 7) / span, 0x88);
    }
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
    D_0038D680.timer--;
}
