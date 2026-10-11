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
    char pad[0x430];
    s16 s430;
    s16 s432;
    s16 s434;
} Ctl;

typedef struct {
    Owner *owner;
    char pad0[0xC - 4];
    Ctl *ctl;
    char pad1[0xC6 - 0x10];
    u16 flags_c6;
    char pad2[0xD8 - 0xC8];
    u64 b0 : 14;
    u64 b14 : 1;
    u64 b15 : 19;
    u64 b34 : 1;
    u64 b35 : 29;
} Actor;

void func_0016ABB8(void *, int);
int func_00192EB0(void *, int);
int func_00194368(float, void *);
void func_00196120(float, void *, void *, int, void *);

void func_0016AA00(Actor *arg0) {
    float sp[4];
    u32 temp_a0;
    s64 temp_a1;
    int m;
    Ctl *c;
    char *temp_a2;

    temp_a1 = *(s64 *)((char *)arg0 + 0xD8);
    temp_a2 = (char *)arg0 + 0xE0;
    if (((int) ((s64) (temp_a1 << 0x12) >> 0x20)) & 1) {
        temp_a0 = arg0->owner->mid->v88;
        if (temp_a0 != 0) {
            arg0->b34 = func_00194368((arg0->flags_c6 & 0x12) ? *(float *)(temp_a2 + 0x40) : 1.0f / (float) temp_a0, arg0->owner);
        } else {
            m = ((int) (temp_a1 >> 11)) & 7;
            arg0->b34 = func_00192EB0(arg0, m);
        }
        func_0016ABB8(arg0, ((int) ((s64) (*(s64 *)((char *)arg0 + 0xD8) << 0x16) >> 0x20)) & 1);
        c = arg0->ctl;
        sp[0] = -c->s430 * 0.017453292f;
        sp[1] = c->s430 * 0.017453292f;
        sp[3] = c->s432 * 0.017453292f;
        sp[2] = -c->s432 * 0.017453292f;
        if (((int) (*(s64 *)((char *)arg0 + 0xD8) >> 9)) & 1) {
            func_00196120(12.0f, arg0, temp_a2 + 0x20, ((int) (*(s64 *)((char *)arg0 + 0xD8) >> 9)) & 1, sp);
        } else {
            func_00196120(8.0f, arg0, temp_a2 + 0x20, -1, 0);
        }
    }
}
