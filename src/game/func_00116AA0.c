#include "common.h"

extern char D_0038CD20[];
extern int D_00446770;
void func_0010A4E8(u8, u8, u8, u8, int);

void func_00116AA0(void) {
    u8 *p;
    char *base;

    base = D_0038CD20 + D_00446770 * 0x60;
    p = *(u8 **)(base + 8);
    func_0010A4E8(p[0x78], p[0x79], p[0x7A], p[0x7B], 0x44);
}
