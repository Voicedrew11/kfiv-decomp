#include "common.h"
#include "kf_gs.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010AEB8(int *packet);

/* Sets up the GS test/alpha/zbuf state, then draws the packet. */
void func_001A3B38(int *packet) {
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
    func_00109A80(GS_REG_ALPHA_1, 0x42);
    func_00109A80(GS_REG_TEST_1, 0x53001);
    func_00109AD0();
    func_0010AEB8(packet);
}
