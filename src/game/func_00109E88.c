#include "kf_gs.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);

void func_00109E88(void) {
    func_00109A50();
    func_00109A80(GS_REG_TEXFLUSH, 0xFFFFFFFFL);
    func_00109AD0();
}
