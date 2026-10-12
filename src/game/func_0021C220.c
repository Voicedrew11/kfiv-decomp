#include "common.h"

typedef struct {
    int f00;
    char pad04[0x28];
    int f2C;
    int f30;
    int f34;
    int f38;
    int f3C;
    int f40;
    int f44;
    int f48;
    int f4C;
    int f50;
    int f54;
    int f58;
} Buf;

void *memset(void *, int, u32);
int sceSifAllocIopHeap(int);
int func_0021CBE8(int, void *, int);
void func_0021CC60(int);
extern int D_00437920[];
extern char D_01E4F7C0[];

int func_0021C220(Buf *arg0, int arg1, int arg2, int arg3) {
    int temp_v0;

    arg0->f00 = 0;
    arg0->f2C = 0;
    arg0->f34 = 0;
    arg0->f38 = 0;
    arg0->f40 = 0;
    arg0->f54 = 0;
    arg0->f4C = 0;
    arg0->f50 = 0;
    arg0->f30 = arg1;
    arg0->f3C = arg2;
    arg0->f48 = arg3;
    temp_v0 = sceSifAllocIopHeap(arg3);
    arg0->f44 = temp_v0;
    if (temp_v0 < 0) {
        return 0;
    }
    temp_v0 = sceSifAllocIopHeap(0x800);
    arg0->f58 = temp_v0;
    if (temp_v0 < 0) {
        return 0;
    }
    memset(D_01E4F7C0, 0, 0x800);
    func_0021CBE8(arg0->f58, D_01E4F7C0, 0x800);
    func_0021CC60(D_00437920[0]);
    return 1;
}
