#include "common.h"

void func_0010AEB8(void *);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);

/* Sends the model's textures and sets GS register 0x47 for drawing it. */
void func_001CA578(void *model) {
    func_0010AEB8(model);
    func_00109A50();
    func_00109A80(0x47, 0x5000D);
    func_00109AD0();
}
