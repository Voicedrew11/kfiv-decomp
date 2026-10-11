#include "common.h"

typedef struct {
    char pad0[0xC];
    int *ctrl;                     /* 0xC */
    char pad1[0xC6 - 0x10];
    u16 flags;          /* 0xC6 */
    char pad2[0xD7 - 0xC8];
    u8 byteD7;          /* 0xD7 */
    u64 bits;       /* 0xD8 */
} Actor;

extern u32 D_00446ED4;

int func_00182DE0(void *, int);
int func_00185460(void *, int);
void func_00196870(void *, int);
void func_00196CC0(void *);

void func_00185390(void *arg0, int arg1) {
    Actor *a = arg0;
    int mask = ~0x20000000;

    a->bits &= mask;
    if (D_00446ED4 % 10U == a->byteD7 % 10U) {
        func_00196870(arg0, 1);
    }
    if (a->flags != 0) {
        return;
    }
    func_00196CC0(arg0);
    if ((int) (a->bits >> 0x35) & 1) {
        func_00182DE0(arg0, arg1);
        return;
    }
    func_00185460(arg0, arg1);
}
