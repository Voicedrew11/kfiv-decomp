#include "kf_movie.h"

extern MovieThreadState D_00437850;

void func_0021B4D0(u32, u32);
void *func_0021B4F0(u32, u32);
void func_0021E6F0(MovieDisplay *, s32, s32, s32, s32, s32, s32);
void func_0021ED48(MovieDisplay *, u32);
int func_0021B728(char *, int (*)(void));
/* the stub takes an argument the retail caller passes and it ignores */
void func_0021E958();

/* Plays a movie file: carves the player's buffers out of the block given, sets up the display and runs
   the movie to its end. Returns -1 when the block is too small. */
int func_0021B540(char *path, u32 heap, u32 heap_size, int (*skip_requested)(void), u32 unkD0, s16 unkD4, s16 unkD6,
                  s16 unkD8) {
    func_0021B4D0(heap, heap_size);
    if ((D_00437850.unk10 = func_0021B4F0(0x800, 0x40)) == NULL ||
        (D_00437850.unk14 = func_0021B4F0(0x4000, 0x40)) == NULL ||
        (D_00437850.unk18 = func_0021B4F0(0x13B800, 0x40)) == NULL ||
        (D_00437850.unk1C = func_0021B4F0(0xC000, 0x40)) == NULL ||
        (D_00437850.unk20 = func_0021B4F0(0x40000, 0x40)) == NULL ||
        (D_00437850.unk24 = func_0021B4F0(0x810, 0x40)) == NULL ||
        (D_00437850.out_pictures = func_0021B4F0(0x578000, 0x40)) == NULL ||
        (D_00437850.out_tags = func_0021B4F0(0x10AF40, 0x40)) == NULL ||
        (D_00437850.unk30 = func_0021B4F0(0x3C00, 0x40)) == NULL ||
        (D_00437850.unk34 = func_0021B4F0(0x100, 0x40)) == NULL ||
        (D_00437850.unk38 = (s32)func_0021B4F0(0x5000C, 0x40)) == 0) {
        return -1;
    }
    D_00437850.unkD4 = unkD4;
    D_00437850.unkD6 = unkD6;
    D_00437850.unkD8 = unkD8;
    func_0021E6F0((MovieDisplay *)D_00437850.unk80, 0, 0, 0x280, 0x1C0, 0x280, 0x1C0);
    func_0021ED48((MovieDisplay *)D_00437850.unk80, 0);
    if (unkD0 >= 0x4000) {
        unkD0 = 0x3FFF;
    }
    D_00437850.unkD0 = unkD0;
    func_0021B728(path, skip_requested);
    func_0021E958(D_00437850.unk80);
    return 0;
}
