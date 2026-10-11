#include "common.h"

typedef struct {
    char pad0[0xC6];
    u16 flags;          /* 0xC6 */
    char pad1[0xD7 - 0xC8];
    u8 byteD7;          /* 0xD7 */
} Actor;

typedef struct {
    char pad0[0x7A];
    s8 f7A;               /* 0xE0 + 0x7A */
} Sub;

extern u32 D_00446ED4;

void func_001311C8(void *, int);
void func_00196870(void *, int);

void func_00131128(void *arg0, int arg1) {
    Actor *a = arg0;
    Sub *s = (Sub *) ((char *) arg0 + 0xE0);

    if (D_00446ED4 % 10U == a->byteD7 % 10U) {
        func_00196870(arg0, 1);
    }
    if (a->flags == 0) {
        func_001311C8(arg0, arg1);
    }
    if (s->f7A > 0) {
        s->f7A--;
    }
}
