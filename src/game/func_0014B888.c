#include "common.h"

typedef struct {
    char pad0[0xC6];
    u16 flags;          /* 0xC6 */
    char pad1[0xCC - 0xC8];
    s8 fCC;               /* 0xCC */
    char pad2[0xD7 - 0xCD];
    u8 byteD7;          /* 0xD7 */
    u64 b0 : 41;         /* 0xD8 */
    u32 b41 : 1;
    u32 b42 : 11;
    u32 b53 : 1;
    u32 b54 : 10;
} Actor;

extern u32 D_00446ED4;

int func_0014B988(void *, int);
int func_0014BA98(void *, int);
int func_00182DE0(void *, int);
void func_0018E9C0(void *);
void func_00196870(void *, int);
int func_00196CC0(void *);

void func_0014B888(void *arg0, int arg1) {
    Actor *a = arg0;
    u32 temp_hi;

    temp_hi = D_00446ED4 % 10U;
    if (temp_hi == a->byteD7 % 10U) {
        func_00196870(arg0, 1);
    }
    if (a->flags != 0) {
        return;
    }
    func_00196CC0(arg0);
    func_0018E9C0(arg0);
    if (a->b53) {
        func_00182DE0(arg0, arg1);
        return;
    }
    switch (a->fCC) {
    case 0:
    case 1:
        break;
    case 2:
        func_0014BA98(arg0, arg1);
        return;
    }
    func_0014B988(arg0, arg1);
}
