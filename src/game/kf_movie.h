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

#endif
