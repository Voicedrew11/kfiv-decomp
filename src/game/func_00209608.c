#include "common.h"

extern int (*D_0042C1A0[4])(void);
extern void (*D_0042C1B0[4])(void);
extern int (*D_004473C4)(void);

int func_00209688(void);
void func_00209D68(void);
void func_00209E48(void);

int func_00209608(int screen) {
    int result;

    func_00209688();
    D_004473C4 = D_0042C1A0[screen];
    D_0042C1B0[screen]();
    do {
        result = D_004473C4();
        func_00209D68();
    } while (result == 0);
    func_00209E48();
    return result;
}
