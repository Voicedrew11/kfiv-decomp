#include "common.h"

typedef struct {
    char pad0[0xEC];
    u16 uEC;
    u16 uEE;
    u16 uF0;
    char pad1[0x26];
    u16 u118;
} Ctl;

typedef struct {
    char pad0[0x48];
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
void func_001AF0B0(void *);
int func_001AF4E0(void *);
void func_001AF5B0(void *, void *);
void func_001A1E28(u16, void *, void *, int, u16, void *);
s32 func_001A2838(u16, void *);
void sceVu0AddVector(void *, void *, void *);

void func_001A7548(Actor *arg0) {
    char *s1;

    func_001AEC48(arg0);
    s1 = (char *)arg0 + 0x10;
    func_001AED58(arg0);
    func_001AF0B0(arg0);
    if (arg0->count == 0) {
        sceVu0AddVector(arg0->p50 + 0xC0, arg0->p50 + 0xC0, s1);
        func_001AF5B0(arg0->p50 + 0xD0, s1);
        if (func_001AF4E0(arg0) != 0) {
            func_001A1E28(arg0->p54->u118, arg0->p50 + 0xC0, arg0->p50 + 0xD0, arg0->i5C, arg0->u4E, arg0->p48);
        } else {
            if (*(u16 *)(s1 + 0x24) == 0) {
                func_001A1E28(arg0->p54->u118, (char *)arg0 + 0x20, arg0->p50 + 0xD0, arg0->i5C, arg0->u4E, arg0->p48);
            } else {
                u16 v;

                if (*(int *)(s1 + 0x20) > 0) {
                    v = arg0->p54->uEC;
                } else {
                    v = arg0->p54->uF0;
                }
                func_001A2838(v, arg0);
            }
        }
    }
    arg0->count++;
}
