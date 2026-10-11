#include "common.h"

typedef struct {
    char pad0[0x10B];
    u8 b10B;
    char pad1[0xC];
    u16 u118;
} Ctl;

typedef struct {
    char pad0[0x48];
    void *p48;
    char pad1[2];
    u16 u4E;
    char *p50;
    Ctl *p54;
    char pad2[4];
    int i5C;
} Actor;

int func_001A2A18(void *);
void func_001A1E28(u16, void *, void *, int, u16, void *);

void func_001AF468(Actor *arg0, char *arg1) {
    char *p50;

    if (*arg1 == 1) {
        func_001A2A18(arg0);
        return;
    }
    p50 = arg0->p50;
    func_001A1E28(arg0->p54->u118, p50 + 0xC0, p50 + 0xD0, arg0->i5C, arg0->u4E, arg0->p48);
    *arg1 = 1;
}
