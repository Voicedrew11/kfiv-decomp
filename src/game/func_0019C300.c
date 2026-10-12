#include "common.h"
#include "kf_sound.h"

extern s32 D_00502170[][2];
extern int D_003EF500[];
extern int D_003EF520[];
extern int D_003EF5B8[];
extern int D_003EF5E8[];
extern s32 D_003EF618[];
extern SoundChannel D_003EF710[];
extern SoundChannel D_00500170[];
extern char D_00500158[];
extern char D_002496BF[];
extern int D_004468BC;
extern float D_004468C0;
extern int D_004468C4;
extern u8 D_004470E0;
extern u8 D_004470E1;
extern u8 D_004470E2;
extern u8 D_004470E3;
extern u8 D_004470E4;
extern u8 D_004470E5;
extern u8 D_004470E6;
extern u8 D_004470E7;

void *memset(void *, int, u32);
int func_001CB778(void);
int func_001CBAE0(int, int, int, int);
void func_001CBE60(void);
void func_001A1458(void *);
int func_0019F720(void);

/* Initialises the sound system: the sound heap, the channel tables and the fade colors. */
void func_0019C300(void) {
    int i;
    u32 heap_start;

    D_004468BC = 0x40;
    D_004468C0 = 0.013f;
    D_004468C4 = 1;
    D_004470E2 = 0;
    D_004470E3 = 0;
    D_004470E1 = 0;
    D_004470E4 = 0xFF;
    D_004470E5 = 0;
    D_004470E6 = 0;
    D_004470E0 = 0;
    D_004470E7 = 0xFF;
    memset(D_00502170, 0, 0xD0);
    func_001CB778();
    func_001CBAE0(1, 0x100, 0xA, 0x800);
    func_001CBE60();
    D_003EF500[2] = 0;
    D_003EF500[0] = heap_start = ((u32)D_002496BF >> 6) << 6;
    D_003EF500[1] = heap_start + 0xA0000;
    func_001A1458(D_003EF5E8);
    D_003EF520[0] = D_003EF500[0];
    D_003EF520[1] = D_003EF500[1];
    func_001A1458(D_003EF618);
    for (i = 0; i < 0x80; i++) {
        D_003EF710[i].unk3A = -1;
    }
    func_0019F720();
    for (i = 0; i < 0x80; i++) {
        D_00500170[i].unk38 = -1;
    }
    for (i = 0; i < 0x10; i++) {
        D_00500158[i] = -1;
    }
    D_003EF5B8[4] = 0;
    D_003EF5B8[5] = 0;
}
