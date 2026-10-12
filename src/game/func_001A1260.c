#include "common.h"

extern char D_002496BF[];
extern int D_003EF580[];
extern int D_003EF6A8[];
extern int D_004470F4;

int sprintf(char *, const char *, ...);
int FlushCache(int);
int func_001A1360(char *, char *, s32 *, int, int);
int func_001A1438();
int func_001CD6B8(int);
int func_001CD7E0(int);

/* Starts loading two named files with the given number into the buffers past the heap start. */
void func_001A1260(int file_number) {
    char path_a[128];
    char path_b[128];
    int heap_start;
    int buffer_end;

    sprintf(path_a, (char *)D_003EF6A8[1], file_number);
    sprintf(path_b, (char *)D_003EF6A8[2], file_number);
    heap_start = ((u32)D_002496BF >> 6) << 6;
    D_003EF580[0] = heap_start;
    buffer_end = heap_start + 0x10000;
    D_003EF580[1] = buffer_end;
    D_003EF580[2] = 0;
    D_004470F4 = buffer_end + 0x8000;
    while (func_001A1438() == 0);
    D_003EF6A8[4] = func_001A1360(path_a, path_b, (s32 *)D_003EF6A8[0], D_003EF6A8[6], D_003EF6A8[7]);
    while (func_001A1438() == 0);
    while (func_001CD6B8(0) == 0);
    func_001CD7E0(D_003EF6A8[4]);
    FlushCache(0);
}
