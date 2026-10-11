#include "common.h"

typedef struct {
    char pad[0x88];
    u32 v88;
} Mid;

typedef struct {
    char pad[0xFC];
    Mid *mid;
} Owner;

typedef struct {
    u64 b0 : 11;
    u64 b11 : 3;
    u64 b14 : 1;
    u64 b15 : 19;
    u64 b34 : 1;
    u64 b35 : 29;
} Flags;

typedef struct {
    Owner *owner;
    char pad1[0xC6 - 4];
    u16 flags_c6;
    char pad2[0xD8 - 0xC8];
    Flags f;
} Actor;

int func_00192EB0(void *, int, void *);
int func_00194368(float, void *);

void func_00135640(Actor *arg0) {
    u32 temp_a0;
    s64 temp_a1;
    char *temp_a2;

    temp_a1 = *(s64 *)&arg0->f;
    temp_a2 = (char *)arg0 + 0xE0;
    if (((int) ((s64) (temp_a1 << 0x12) >> 0x20)) & 1) {
        arg0->f.b11 = 0;
        temp_a0 = arg0->owner->mid->v88;
        if (temp_a0 != 0) {
            arg0->f.b34 = func_00194368((arg0->flags_c6 & 0x12) ? *(float *)(temp_a2 + 0x40) : 1.0f / (float) temp_a0, arg0->owner);
        } else {
            arg0->f.b34 = func_00192EB0(arg0, arg0->f.b11, temp_a2);
        }
    }
}
