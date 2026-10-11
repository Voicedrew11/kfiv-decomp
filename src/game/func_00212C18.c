#include "common.h"

typedef struct {
    int a00;
    int a04;
    int a08;
} Obj;

int func_0011E1F0(int, int);
void func_00211CF8(char *, int);
extern int D_00446770;

void func_00212C18(Obj *arg0) {
    int r;

    r = func_0011E1F0(D_00446770, arg0->a08);
    if (r != 0) {
        func_00211CF8((char *)arg0 + 0xC, (*(u32 *)(r + 0x10) >> 17) & 1);
    } else {
        func_00211CF8((char *)arg0 + 0xC, 0);
    }
    arg0->a00 = 0;
}
