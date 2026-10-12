#include "kf_gs.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010A4E8(int, int, int, int, int);

/* Map change drawing step: keeps the screen black. */
void func_00118C28(void) {
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x30000);
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109AD0();
    func_0010A4E8(0, 0, 0, 0x80, 0x88);
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
}
