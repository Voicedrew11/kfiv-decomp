#include "common.h"

void func_001CE620(s32, s32);
extern s32 D_003EF5CC[];

void func_0019C7D0(s32 level) {
    func_001CE620(D_003EF5CC[0], (s16)(level < 0x80 ? level : 0x7F));
}
