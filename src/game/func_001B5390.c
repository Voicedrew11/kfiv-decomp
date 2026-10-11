#include "common.h"

int func_00216F50(int);
int func_00216FB8(int, int);
void func_00217008(void);
void *memset(void *, int, u32);
extern u16 D_00413DA8[];
extern char D_00413DB8[];

void func_001B5390(void) {
    func_00216F50(0);
    func_00216FB8(0, 0);
    func_00217008();
    memset(D_00413DA8, 0, 0x10);
    memset(D_00413DB8, 0, 0x20);
}
