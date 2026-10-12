#include "common.h"

void func_001CBED8(void);
void func_001CC018(void);

extern u8 D_00349680[];

void func_0019C520(void) {
    if (D_00349680[0] != 0) {
        func_001CC018();
        return;
    }
    func_001CBED8();
}
