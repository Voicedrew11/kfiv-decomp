#include "kf_gs.h"
#include "kf_model.h"

void func_0010AEB8(void *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);

void func_002095D0(ModelInstance *model) {
    func_0010AEB8(model);
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x5010D);
    func_00109AD0();
}
