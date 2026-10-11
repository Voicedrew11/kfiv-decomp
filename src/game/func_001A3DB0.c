#include "common.h"

void func_00109A50(void);
void func_00109A80(int, u64);
void func_00109AD0(void);
void func_0010AEB8(int *arg0);

void func_001A3DB0(int *arg0) {
    func_00109A50();
    func_00109A80(0x4E, 0xa000118L);
    func_00109A80(0x42, 0x80000000a8L);
    func_00109A80(0x47, 0x3000D);
    func_00109AD0();
    func_0010AEB8(arg0);
}
