#include "common.h"

typedef struct {
    char pad[0x88];
    u32 v88;
} Mid;

typedef struct {
    char pad[0xD4];
    float fD4;
    char pad2[0xFC - 0xD8];
    Mid *mid;
    char pad3[0x138 - 0x100];
    char *p138;
} Owner;

typedef struct {
    Owner *owner;
    char pad1[0x68 - 4];
    float f68;
    char pad4[0xC6 - 0x6C];
    u16 flags_c6;
    char pad5[0xD1 - 0xC8];
    u8 state;
    char pad2[0xD8 - 0xD2];
    u64 b0 : 14;
    u64 b14 : 1;
    u64 b15 : 19;
    u64 b34 : 1;
    u64 b35 : 29;
} Actor;

int func_00192EB0(void *, int);
int func_00194368(float, void *);
float func_001957B8(float, float);
float func_00195388(float *, float, float);

void func_0012A0A8(Actor *arg0) {
    u32 temp_a0;
    s64 temp_a1;
    int m;
    char *temp_a2;
    char *p;
    float a;

    temp_a2 = (char *)arg0 + 0xE0;
    temp_a1 = *(s64 *)((char *)arg0 + 0xD8);
    p = arg0->owner->p138;
    if (((int) ((s64) (temp_a1 << 0x12) >> 0x20)) & 1) {
        temp_a0 = arg0->owner->mid->v88;
        if (temp_a0 != 0) {
            arg0->b34 = func_00194368((arg0->flags_c6 & 2) ? *(float *)(temp_a2 + 0x40) : 1.0f / (float) temp_a0, arg0->owner);
        } else {
            m = ((int) (temp_a1 >> 11)) & 7;
            arg0->b34 = func_00192EB0(arg0, m);
        }
        if (p != 0) {
            if ((u8) (arg0->state - 0xE) < 3) {
                *(float *)(temp_a2 + 0x24) = -*(float *)(p + 0x54);
            } else {
                a = func_001957B8(arg0->owner->fD4, arg0->f68);
                if (a < -0.5235988f) {
                    a = -0.5235988f;
                }
                if (0.5235988f < a) {
                    a = 0.5235988f;
                }
                func_00195388((float *)(temp_a2 + 0x24), a, 10.0f);
                *(float *)(p + 0x54) = -*(float *)(temp_a2 + 0x24);
            }
        }
    }
}
