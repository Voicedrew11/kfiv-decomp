#include "common.h"

int func_001CE620(int, s16);
extern s32 D_003EF5CC[];

void func_0019C7D0(s32 level) {
    func_001CE620(D_003EF5CC[0], (s16)(level < 0x80 ? level : 0x7F));
}
