#include "common.h"

typedef struct {
    char pad[0x88];
    u32 v88;
} Mid;

typedef struct {
    char pad[0xFC];
    Mid *mid;
} Owner;

typedef struct { s64 v; } __attribute__((packed)) U64P;

typedef struct {
    char a;
    char pad[0x1F];
    u16 x20;
    s16 x22;
    char pad2[4];
    U64P x28;
} P;

typedef struct {
    Owner *owner;
    char pad0[0xC - 4];
    u32 *ctl;
    char pad1[0xC6 - 0x10];
    u16 flags_c6;
    char pad2[0xD8 - 0xC8];
    u64 b0 : 14;
    u64 b14 : 1;
    u64 b15 : 19;
    u64 b34 : 1;
    u64 b35 : 29;
} Actor;

int func_00192EB0(void *, int);
int func_00194368(float, void *);
void func_0014B178(void *);
void *func_001B1390();
char *func_0010CAB8(char *, u32);
void func_001B4F30(u8, void *, void *, void *, void *);
void func_0018D600(char **, int, u32, int);
extern u32 D_00446ED4;

void func_0014B270(Actor *arg0) {
    u32 temp_a0;
    s64 temp_a1;
    int m;
    char *temp_a2;
    P *p;
    float unused[4]; /* unused, but retail reserves the 0x10 bytes */

    temp_a1 = *(s64 *)((char *)arg0 + 0xD8);
    temp_a2 = (char *)arg0 + 0xE0;
    if (((int) ((s64) (temp_a1 << 0x12) >> 0x20)) & 1) {
        if ((*arg0->ctl & 0x6000000) == 0x6000000) {
            *(s64 *)((char *)arg0 + 0xD8) = temp_a1 & ~0x3800L;
        }
        temp_a0 = arg0->owner->mid->v88;
        if (temp_a0 != 0) {
            arg0->b34 = func_00194368((arg0->flags_c6 & 0x12) ? *(float *)(temp_a2 + 0x40) : 1.0f / (float) temp_a0, arg0->owner);
        } else {
            m = ((int) (*(s64 *)((char *)arg0 + 0xD8) >> 11)) & 7;
            arg0->b34 = func_00192EB0(arg0, m);
        }
        if (arg0->flags_c6 == 0 && arg0->owner->mid->v88 == 0) {
            func_0014B178(arg0);
        } else {
            *(int *)(temp_a2 + 0x50) = 0;
        }
        if (D_00446ED4 % 10U == 0) {
            p = func_001B1390();
            p->a = 1;
            p->x20 = 0;
            p->x22 = 0;
            p->x28 = *(U64P *)((char *)arg0->ctl + 0x238);
            func_001B4F30(0xD0, p, func_0010CAB8((char *)arg0->owner, 0xFF), (char *)arg0->owner + 0xD0, arg0);
        } else {
            func_0018D600((char **)arg0, 0x39D, 0xFF, (int)((char *)arg0->owner + 0xD0));
        }
    }
}
