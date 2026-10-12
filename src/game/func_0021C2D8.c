#include "kf_movie.h"

int sceSifFreeIopHeap(int);
int printf(char *, ...);
void func_0021CC60(int);
extern char D_004420F8[];
extern char D_00442118[];

int func_0021C2D8(MovieBuffers *movie) {
    if (sceSifFreeIopHeap(movie->iop_buffer0) < 0) {
        printf(D_004420F8, D_00442118, 0x3AE);
    }
    if (sceSifFreeIopHeap(movie->iop_buffer1) < 0) {
        printf(D_004420F8, D_00442118, 0x3B0);
    }
    func_0021CC60(0);
    return 1;
}
