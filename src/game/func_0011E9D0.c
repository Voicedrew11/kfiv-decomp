#include "common.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_0010AEB8(void *);

/* Sets the GS registers for drawing a map object's model, then draws it. */
void func_0011E9D0(void *model) {
    func_00109A50();
    func_00109A80(0x4E, 0x10A000118);
    func_00109A80(0x4F, 0x10A000118);
    func_00109A80(0x42, 0x44);
    func_00109A80(0x43, 9);
    func_00109A80(0x47, 0x5000D);
    func_00109A80(0x48, 0x5000D);
    func_00109AD0();
    func_0010AEB8(model);
}
