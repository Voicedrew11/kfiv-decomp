#include "common.h"

typedef struct Obj {
    char pad0[0xC];
    float f0C;
    char pad10[0x4C - 0x10];
    u16 u4C;
    char pad4E[2];
    char *p50;
    char *p54;
    char pad58[8];
    struct Obj *p60;
} Obj;

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
void func_00216778(void *, void *);

void func_001AE498(Obj *arg0) {
    char *temp_s0;
    char *temp_s1;
    int temp_v0;
    Obj *temp_v1;

    temp_s0 = (char *)arg0 + 0x10;
    func_001AEC48(arg0);
    temp_s1 = (char *)arg0 + 0x20;
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    temp_v1 = (Obj *)arg0->p60;
    arg0->f0C = arg0->f0C * (*(float *)((char *)temp_v1 + 0xC) * 0.0078125f);
    func_00216778(arg0->p50 + 0xC0, temp_s0);
    func_00216778(arg0->p50 + 0xD0, temp_s1);
    func_00216778(temp_s0, arg0->p60->p50 + 0xC0);
    func_00216778(temp_s1, arg0->p60->p50 + 0xD0);
    temp_v0 = arg0->u4C + 1;
    arg0->u4C = temp_v0;
    if ((u32)(temp_v0 & 0xFFFF) >= (u32)*(u16 *)(arg0->p54 + 0x102)) {
        arg0->u4C = 0;
    }
}
