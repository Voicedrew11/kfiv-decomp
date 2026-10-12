#include "common.h"

extern int D_004473C8;
extern int D_004473CC;
extern void (*D_004473C4)(void);
extern int D_005FAD60[];
extern int D_005FAD80[];

int func_00209A48(float, void *, int);
int func_00209FC8(void);
void func_00217008(void);
void *func_00217760(int, int);
int func_0020A108(void);

int func_00209EE8(void) {
    int var_v1;
    char *temp_s0;

    D_004473C8 -= 1;
    func_00217008();
    temp_s0 = func_00217760(0, 0);
    var_v1 = D_004473CC;
    if (var_v1 == 0) {
        if (D_004473C8 <= 0) {
            var_v1 = 1;
            D_004473C8 = 0x12C;
            D_004473CC = 1;
        }
    }
    if (var_v1 == 0) {
        func_00209A48(1.0f, D_005FAD80, 0);
    } else {
        func_00209A48(1.0f, D_005FAD60, 0);
    }
    if (!((*(u16 *)(temp_s0 + 0x20) & 8) || (D_004473C8 <= 0 && D_004473CC == 1))) {
        return 0;
    }
    D_004473C4 = func_0020A108;
    func_00209FC8();
    return 0;
}
