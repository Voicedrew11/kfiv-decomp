#include "common.h"

extern char D_00474AC0[];
extern char D_00474B60[];

void func_0010A880(void *, void *);
void func_0010A008(void);
void func_00109E88(void);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);

void func_0011ED18(void) {
    int (*entry)[10];
    int *value;

    entry = (void *)D_00474AC0;
    value = (int *)D_00474B60;
    func_0010A880(entry++, value++);
    func_0010A880(entry++, value++);
    func_0010A880(entry++, value++);
    func_0010A880(entry++, value++);
    func_00109E88();
    func_0010A008();
    func_00109A50();
    func_00109A80(0x42, 0x44);
    func_00109A80(0x47, 0x5000D);
    func_00109AD0();
}
