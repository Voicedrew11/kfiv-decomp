#include "common.h"
#include "kf_effect.h"

extern u32 *D_004470F8;

void func_00109E18(void);
u32 *func_00109990(int);
void func_00109650(u32 *, u32 *);
void func_00109E88(void);

void func_001A2108(void) {
    u32 *first_word;
    u32 *cursor;

    func_00109E18();
    first_word = cursor = func_00109990(2);
    *cursor++ = D_004470F8[0] | 0x30000000;
    *cursor++ = (u32)(D_004470F8 + 4);
    *cursor++ = 0;
    *cursor = 0;
    func_00109650(first_word, cursor + 1);
    func_00109E88();
}
