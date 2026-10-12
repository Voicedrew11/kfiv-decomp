#include "common.h"

typedef struct Obj {
    char pad0[0x10];
    int i10;
    char pad14[0x4C - 0x14];
    u16 u4C;
    char pad4E[2];
    char *p50;
    char *p54;
} Obj;

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
extern u8 D_0034AFD6[];

void func_001AEAB0(Obj *arg0) {
    func_001AEC48(arg0);
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    arg0->u4C = arg0->u4C + 1;
    if (D_0034AFD6[0] == 3) {
        arg0->i10 = 1;
    }
    if (arg0->i10 == 0) {
        if (arg0->u4C >= *(u16 *)(arg0->p54 + 0xEC)) {
            arg0->u4C = 0;
        }
    }
}
