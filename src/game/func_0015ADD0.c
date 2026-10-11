#include "common.h"

void func_00190A30(char *);
void func_00195898(char *, int);
void func_0019A310(char *, void (*)(void), int);
void func_0015ACA8(char *);
void func_00181378(void);
void func_001814B0(void);
void func_001816A0(void);

typedef struct {
    u64 z0 : 9;
    u64 b9 : 1;
    u64 z10 : 1;
    u64 z11 : 53;
} Flags;

typedef struct {
    char pad0[0x5C];
    int i5C;
    char pad60[0xD8 - 0x60];
    union { Flags b; u64 l; } flags;
} Actor;

void func_0015ADD0(Actor *arg0) {
    u64 temp_v0;

    func_00190A30((char *)arg0);
    arg0->i5C = 0;
    arg0->flags.b.z10 = 0;
    arg0->flags.b.b9 = 0;
    temp_v0 = arg0->flags.l;
    if (((int)(temp_v0 >> 36) & 3) == 1) {
        if ((int)((s64)(temp_v0 << 13) >> 32) & 1) {
            func_0019A310((char *)arg0, func_00181378, 0);
        } else {
            func_0019A310((char *)arg0, func_001814B0, 0);
        }
    } else if (((int)(temp_v0 >> 36) & 3) == 2) {
        func_0019A310((char *)arg0, func_001816A0, 0);
    }
    func_00195898((char *)arg0, 2);
    func_0015ACA8((char *)arg0);
}
