#include "common.h"

typedef struct {
    char pad0[0x10B];
    u8 b10B;
} Ctl;

typedef struct {
    char pad0[0x4E];
    u16 u4E;
    char pad1[4];
    Ctl *p54;
} Actor;

int func_001B4F90(void *);

int func_001AF4E0(Actor *arg0) {
    if (arg0->u4E != 0xFFFF) {
        if (arg0->p54->b10B != 0xFF) {
            if (func_001B4F90(arg0) != 0) {
                return 1;
            }
        }
    }
    return 0;
}
