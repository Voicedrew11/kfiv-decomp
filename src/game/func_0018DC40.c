#include "common.h"

void func_001251E0(void);
void func_001A32A8(void *, int, int);
char *func_0010CAB8(char *, u32);
int func_001A1C58(int, void *, void *, void *);

void func_0018DC40(char *arg0) {
    char *r;

    func_001251E0();
    *(s64 *)(arg0 + 0xD8) = ((*(s64 *)(arg0 + 0xD8) | 2) & ~0xC) | 0x100;
    func_001A32A8(arg0, -0xD, 0x100);
    if (*(u16 *)(*(char **)(arg0 + 0xC) + 0xC6) != 0) {
        if (*(u16 *)(*(char **)(arg0 + 0xC) + 0xC6) != 0xFFFF) {
            r = func_0010CAB8(*(char **)arg0, *(u8 *)(*(char **)(arg0 + 0xC) + 0xC5));
            func_001A1C58(*(u16 *)(*(char **)(arg0 + 0xC) + 0xC6), r, *(int *)arg0 + 0xD0, arg0);
        }
    }
}
