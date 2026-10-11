#include "common.h"

typedef struct {
    char pad0[0xC6];
    u16 flags;          /* 0xC6 */
    char pad1[0xD7 - 0xC8];
    u8 byteD7;          /* 0xD7 */
} Actor;

typedef struct {
    char pad0[0x68];
    s16 timer;                   /* 0xE0 + 0x68 */
} Sub;

extern u32 D_00446ED4;

void func_00167498(void *, int);
void func_0018E9C0(void *);
void func_00196870(void *, int);

void func_001673F0(void *arg0, int arg1) {
    Actor *a = arg0;
    Sub *s = (Sub *) ((char *) arg0 + 0xE0);

    if (D_00446ED4 % 10U == a->byteD7 % 10U) {
        func_00196870(arg0, 1);
    }
    if (a->flags == 0) {
        func_0018E9C0(arg0);
        func_00167498(arg0, arg1);
    }
    if (s->timer != 0) {
        s->timer--;
    }
}
