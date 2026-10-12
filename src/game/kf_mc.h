/* Memory card: the request state block at D_01E4F278. */
#ifndef KF_MC_H
#define KF_MC_H

#include "common.h"

typedef struct {
    int unk00[32];                  /* 0x00 */
    int unk80;                      /* 0x80 0..2 */
    int unk84;                      /* 0x84 0..4 */
    int state;                      /* 0x88 the request in progress, 0 when idle */
    int unk8C;                      /* 0x8C */
    int unk90;                      /* 0x90 */
    int unk94;                      /* 0x94 */
    int timeout;                    /* 0x98 frames */
    int unk9C;                      /* 0x9C */
    int unkA0;                      /* 0xA0 */
    int unkA4;                      /* 0xA4 */
    int unkA8;                      /* 0xA8 */
    int unkAC;                      /* 0xAC */
    int unkB0;                      /* 0xB0 */
    int unkB4;                      /* 0xB4 */
    int unkB8;                      /* 0xB8 */
} McState;

/* the first 0x80 bytes of McState: what sceMcGetInfo reported per port and slot */
typedef struct {
    int type[2][4];                 /* 0x00 card type */
    int free_clusters[2][4];        /* 0x20 */
    int format[2][4];               /* 0x40 nonzero when formatted */
    int result[2][4];               /* 0x60 result of the sceMcGetInfo call */
} McCardInfo;

#define MC_CARD_INFO ((McCardInfo *)&D_01E4F278)

extern McState D_01E4F278;

/* the PS2 memory card icon.sys header, built at *D_00447338 by func_001FB2A0 */
typedef struct {
    char head[4];                   /* 0x00 "PS2D" */
    char pad04[0x02];
    u16 line_break;                 /* 0x06 */
    char pad08[0x04];
    s32 transparency;               /* 0x0C */
    s32 bg_color[4][4];             /* 0x10 */
    f32 light_dir[3][4];            /* 0x50 */
    f32 light_color[3][4];          /* 0x80 */
    f32 ambient[4];                 /* 0xB0 */
    char title[68];                 /* 0xC0 */
    char view_name[64];             /* 0x104 */
    char copy_name[64];             /* 0x144 */
    char delete_name[64];           /* 0x184 */
    char pad1C4[0x200];
} McIconSys;

extern McIconSys *D_00447338;

/* slot is the memory card slot; the header is the same for both */
void func_001FB2A0(s32 slot);

/* progress of the file being written in 0x400-byte blocks */
typedef struct {
    int total_size;                 /* 0x00 size of the whole file, a multiple of 0x20 */
    u32 offset;                     /* 0x04 bytes of the data done so far */
    int chunk_size;                 /* 0x08 bytes to hand to sceMcWrite this step */
} McFileProgress;

/* the open file and the block being built for it */
typedef struct {
    int file_fd;                    /* 0x00 */
    u8 buffer[0x420];               /* 0x04 */
} McFileWork;

extern McFileProgress D_01E4F240;
extern McFileWork D_01E4F334;

#endif
