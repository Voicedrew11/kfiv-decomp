#include "common.h"

typedef struct {
    char pad0[0xC];
    int *ctrl;                     /* 0xC */
    char pad1[0xC6 - 0x10];
    u16 flags;          /* 0xC6 */
    char pad2[0xD5 - 0xC8];
    s8 byteD5;            /* 0xD5 */
    char pad3[0xD7 - 0xD6];
    u8 byteD7;          /* 0xD7 */
    u64 bits;       /* 0xD8 */
} Actor;

typedef struct {
    char pad0[0x78];
    s8 f78;               /* 0x78 */
    s8 f79;               /* 0x79 */
} Sub;

extern u32 D_00446ED4;

void *func_001267E8(char, int);
int func_0013ED58(void *, int);
int func_0013EE40(void *, int);
int func_0013EFB8(void *, int);
void func_0013F0D8(void *, int);
void func_00196870(void *, int);

void func_0013EC40(void *arg0, int arg1) {
    Actor *a = arg0;
    Actor *t;
    Sub *s = (Sub *) ((char *) arg0 + 0xE0);
    u32 temp_hi;

    t = func_001267E8(a->byteD5, 0x29);
    if (t != 0 && (int) t->bits & 1) {
        if ((t->bits & 0xE) != 2) {
            void (*fp)(void *, int) = func_0013F0D8;
            *(void (**)(void *, int))((char *) arg0 + 0xE0) = fp;
            fp(arg0, arg1);
            return;
        }
    }
    temp_hi = D_00446ED4 % 10U;
    if (temp_hi == a->byteD7 % 10U) {
        func_00196870(arg0, 1);
    }
    if (a->flags == 0) {
        if (s->f78 == 0) {
            func_0013ED58(arg0, arg1);
        } else {
            if (s->f79 != 0) {
                func_0013EE40(arg0, arg1);
            } else {
                func_0013EFB8(arg0, arg1);
            }
        }
    }
}
