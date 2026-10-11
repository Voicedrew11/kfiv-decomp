#include "common.h"

int func_001A2A18(void *);
int func_001AEC48(void *);
int func_001AED58(void *);
int func_001AF0B0(void *);

void func_001ACDA8(void *arg0) {
    if (((char *)arg0)[0x10] != 0) {
        return (void)func_001A2A18(arg0);
    }
    func_001AEC48(arg0);
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    *(u16 *)((char *)arg0 + 0x4C) = (u16)(*(u16 *)((char *)arg0 + 0x4C) + 1);
}
