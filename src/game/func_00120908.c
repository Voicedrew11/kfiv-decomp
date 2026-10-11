#include "common.h"

typedef struct {
    char pad[0x24];
    u32 b0 : 1;
    u32 b1 : 1;
} A;

typedef struct {
    char pad[0x50];
    void *cur;
} B;

void func_001207D0(void *);
void func_00120868(void *);
extern A D_0038CC50;
extern B D_00420D00;

void func_00120908(char *arg0, int arg1) {
    void *saved;
    int bit;

    if ((*(u32 *)(arg0 + 0x10) & 0x2FF00) == 0x100 && arg1 != 0) {
        saved = D_00420D00.cur;
        bit = D_0038CC50.b1;
        D_0038CC50.b1 = 1;
        D_00420D00.cur = arg0;
        func_001207D0(arg0);
        D_00420D00.cur = saved;
        D_0038CC50.b1 = bit;
    } else if ((*(u32 *)(arg0 + 0x10) & 0x2FF00) == 0x20300 && arg1 == 0) {
        saved = D_00420D00.cur;
        bit = D_0038CC50.b1;
        D_0038CC50.b1 = 1;
        D_00420D00.cur = arg0;
        func_00120868(arg0);
        D_00420D00.cur = saved;
        D_0038CC50.b1 = bit;
    }
}
