#include "common.h"

typedef struct {
    char pad0[0xF2];
    u16 uF2;
    char pad1[0x17];
    u8 b10B;
    char pad2[0xC];
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
int func_001B4F90(void *);
void func_001A1E28(u16, void *, void *, int, u16, void *);

int func_001AF358(Actor *arg0, char *arg1) {
    int r;
    char *p50;

    if (*arg1 == 1) {
        func_001A2A18(arg0);
        *arg1 = 0;
        return 0;
    }
    if (arg0->u4E != 0xFFFF && arg0->p54->b10B != 0xFF) {
        r = func_001B4F90(arg0);
        if (r & 2) {
            if (arg0->p54->uF2 != 0) {
                p50 = arg0->p50;
                func_001A1E28(arg0->p54->uF2, p50 + 0xC0, p50 + 0xD0, arg0->i5C, arg0->u4E, arg0->p48);
            }
            return 1;
        }
        if (r != 0) {
            if (arg0->p54->u118 != 0xFFFF) {
                p50 = arg0->p50;
                func_001A1E28(arg0->p54->u118, p50 + 0xC0, p50 + 0xD0, arg0->i5C, arg0->u4E, arg0->p48);
                *arg1 = 1;
            }
            return 2;
        }
    }
    *arg1 = 0;
    return 0;
}
