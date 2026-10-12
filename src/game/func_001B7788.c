#include "common.h"

typedef void (*StateHandler)(u16, u16);

void func_001B7DF8(void);
extern u8 D_004141D8[];
extern StateHandler D_00413F60[];

void func_001B7788(u16 target_id, u16 command) {
    func_001B7DF8();
    D_00413F60[D_004141D8[0]](target_id, command);
}
