#include "common.h"

typedef struct {
    char pad0[0xF0];
    u16 uF0;
} Ctl;

typedef struct {
    char pad0[0x10];
    int i10;
    char pad1[0x48 - 0x14];
    void *p48;
    u16 count;
    u16 u4E;
    char *p50;
    Ctl *p54;
    char pad2[4];
    int i5C;
} Actor;

void func_001AEC48(void *);
void func_001AED58(void *);
int func_001AF0B0(void *);
int func_001A2A18(void *);
void func_001A1E28(u16, void *, void *, int, u16, void *);

void func_001AB940(Actor *arg0) {
    char *p50;

    func_001AEC48(arg0);
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    if (arg0->i10 < arg0->count) {
        p50 = arg0->p50;
        func_001A1E28(arg0->p54->uF0, p50 + 0xC0, p50 + 0xD0, arg0->i5C, arg0->u4E, arg0->p48);
        func_001A2A18(arg0);
    }
    arg0->count = arg0->count + 1;
}
