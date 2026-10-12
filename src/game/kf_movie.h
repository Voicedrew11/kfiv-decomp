/* Movie playback (libmpeg/IPU): the decoder's IOP-side buffers. */
#ifndef KF_MOVIE_H
#define KF_MOVIE_H

#include "common.h"

/* what func_0021C2D8 frees when playback ends */
typedef struct {
    int unk00;                      /* 0x00 */
    char pad04[0x40];
    int iop_buffer0;                /* 0x44 IOP heap block */
    char pad48[0x10];
    int iop_buffer1;                /* 0x58 IOP heap block */
} MovieBuffers;

/* D_00437850: the movie player's threads and the buffer pointer it frees */
typedef struct {
    char pad00[0x38];
    s32 unk38;                      /* 0x38 */
    char pad3C[0x04];
    s32 unk40;                      /* 0x40 thread id */
    s32 unk44;                      /* 0x44 thread id */
    char pad48[0x04];
    void *unk4C;                    /* 0x4C */
} MovieThreadState;

/* D_00437938: the INTC and DMAC handler ids of the movie player */
typedef struct {
    char pad00[0xB0];
    s32 unkB0;                      /* 0xB0 DMAC handler id */
    s32 unkB4;                      /* 0xB4 INTC handler id */
} IntcDmacState;

#endif
