#include "common.h"
#include "kf_sound.h"

extern char D_002496BF[];
extern SoundFileBuffer D_003EF560;
extern SoundJob D_003EF678;
extern int D_004470F4;

int sprintf(char *, const char *, ...);
int FlushCache(int);
int func_001A1360(char *, char *, SoundJobSpec *, int, int);
int func_001A1438(void);
int func_001CD6B8(int);
int func_001CD7E0(int);

/* Builds the two file names for file_id (+0x96 in the name), loads them and starts the effect. */
void func_001A1160(int file_id) {
    char path_a[128];
    char path_b[128];
    int file_number;
    int buffer_base;
    int buffer_limit;

    file_number = file_id + 0x96;
    sprintf(path_a, D_003EF678.name_a, file_number);
    sprintf(path_b, D_003EF678.name_b, file_number);
    buffer_base = ((u32)D_002496BF >> 6) << 6;
    D_003EF560.base = buffer_base;
    buffer_limit = buffer_base + 0x20000;
    D_003EF560.limit = buffer_limit;
    D_003EF560.used = 0;
    D_004470F4 = buffer_limit + 0x8000;
    while (func_001A1438() == 0) {
    }
    D_003EF678.result = func_001A1360(path_a, path_b, D_003EF678.spec, D_003EF678.unk18, D_003EF678.unk1C);
    while (func_001A1438() == 0) {
    }
    while (func_001CD6B8(0) == 0) {
    }
    func_001CD7E0(D_003EF678.result);
    FlushCache(0);
}
