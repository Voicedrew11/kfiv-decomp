#include "common.h"

int func_001A4480(void *);
int rand(void);

typedef struct {
    char pad0[0x4C];
    u16 s4C;
    char pad4E[0x50 - 0x4E];
    struct {
        char pad0[0xD8];
        float fD8;
    } *p50;
} Obj;

void func_001A4410(Obj *arg0) {
    if (arg0->s4C == 0) {
        arg0->p50->fD8 = ((float) (rand() & 0xFFFF) * 0.000030517578125f - 1.0f) * 3.1415f;
    }
    func_001A4480(arg0);
}
