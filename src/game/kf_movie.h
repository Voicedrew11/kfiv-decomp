/* Movie playback (libmpeg/IPU): the decoder's IOP-side buffers. */
#ifndef KF_MOVIE_H
#define KF_MOVIE_H

#include "common.h"

/* what func_0021C2D8 frees when playback ends */
typedef struct {
    int unk00;                      /* 0x00 MOVIE_AUDIO_STATE_* */
    u8 header[0x28];                /* 0x04 the stream's "SShd" and "SSbd" headers */
    int header_count;               /* 0x2C header bytes received */
    u8 *data;                       /* 0x30 ring of PCM data on the EE */
    int put;                        /* 0x34 where the next data goes */
    int count;                      /* 0x38 bytes waiting */
    int size;                       /* 0x3C ring size */
    int total_bytes;                /* 0x40 */
    int iop_buffer0;                /* 0x44 IOP heap block */
    int iop_buffer_size;            /* 0x48 */
    int iop_last_pos;               /* 0x4C where the next block goes in the IOP buffer */
    int iop_pause_pos;              /* 0x50 */
    int total_bytes_sent;           /* 0x54 */
    int iop_buffer1;                /* 0x58 IOP heap block */
} MovieBuffers;

#define MOVIE_AUDIO_STATE_INIT 0
#define MOVIE_AUDIO_STATE_PRESET 1
#define MOVIE_AUDIO_STATE_PLAY 2
#define MOVIE_AUDIO_STATE_PAUSE 3

/* the ring the movie file is read into before it is demultiplexed (the SDK sample's ReadBuf) */
typedef struct {
    u8 data[0x50000];               /* 0x00000 */
    int put;                        /* 0x50000 where the next data goes */
    int count;                      /* 0x50004 bytes waiting */
    int size;                       /* 0x50008 */
} MovieReadBuffer;

/* a source-chain DMA tag in the low half of a quadword */
typedef struct {
    u64 tag;                        /* 0x00 address << 32 | id << 28 | qwc */
    u64 unk08;                      /* 0x08 */
} MovieDmaTag;

/* D_00437850: the movie player's threads and the buffer pointer it frees */
typedef struct {
    u32 heap_start;                 /* 0x00 the block func_0021B540 is given to allocate from */
    u32 heap_next;                  /* 0x04 first free byte (func_0021B4F0) */
    u32 heap_end;                   /* 0x08 last byte */
    char pad0C[0x04];
    void *unk10;                    /* 0x10 0x800 bytes */
    void *unk14;                    /* 0x14 0x4000 bytes */
    void *unk18;                    /* 0x18 0x13B800 bytes */
    void *unk1C;                    /* 0x1C 0xC000 bytes */
    void *unk20;                    /* 0x20 0x40000 bytes */
    void *unk24;                    /* 0x24 0x810 bytes */
    void *out_pictures;             /* 0x28 five MovieOutPicture */
    void *out_tags;                 /* 0x2C five MovieOutTag */
    void *unk30;                    /* 0x30 0x3C00 bytes */
    void *unk34;                    /* 0x34 0x100 bytes */
    s32 unk38;                      /* 0x38 the MovieReadBuffer */
    char pad3C[0x04];
    s32 unk40;                      /* 0x40 thread id */
    s32 unk44;                      /* 0x44 thread id */
    char pad48[0x04];
    void *unk4C;                    /* 0x4C */
    char pad50[0x30];
    char unk80[0x50];               /* 0x80 D_004378D0: what func_0021E6F0 sets up */
    u32 unkD0;                      /* 0xD0 at most 0x3FFF */
    s16 unkD4;                      /* 0xD4 */
    s16 unkD6;                      /* 0xD6 */
    s16 unkD8;                      /* 0xD8 */
} MovieThreadState;

/* MovieThreadState.unk38 as the pointer it holds: func_0021BA20 only matches when the load
   has a pointer type (it then moves ahead of the stores to the DMA registers) */
typedef struct {
    char pad00[0x38];
    MovieReadBuffer *read_buffer;   /* 0x38 */
} MovieThreadStatePointers;

#define MOVIE_READ_BUFFER(state) (((MovieThreadStatePointers *)(state))->read_buffer)

/* the movie file being played (at D_00437850 + 0x4C): streamed from the disc
   when its path starts with "cdrom0:", read with sceRead otherwise */
typedef struct {
    s32 is_cd_stream;               /* 0x00 */
    s32 size;                       /* 0x04 bytes */
    u32 lsn;                        /* 0x08 an sceCdlFILE starts here: first sector */
    u32 cd_size;                    /* 0x0C bytes, from the disc directory */
    char pad10[0x1C];               /* 0x10 rest of the sceCdlFILE */
    s32 iop_buffer;                 /* 0x2C IOP heap block the stream uses as its ring buffer */
    s32 file_fd;                    /* 0x30 host file */
} MovieFile;

/* one decoded picture as the IPU delivers it (RGBA32 macroblocks) */
typedef struct {
    u8 pixels[0x118000];
} MovieOutPicture;

/* what goes with one decoded picture; the GS packets that upload it follow the status */
typedef struct {
    s32 status;                     /* 0x00 MOVIE_OUT_STATUS_* */
    char pad04[0x3C];
    u8 field_packets[2][0x1AB00];   /* 0x40 GIF chain per field: [0] shown on the even field, [1] on the odd */
} MovieOutTag;

#define MOVIE_OUT_STATUS_FULL 2

/* D_00437838: ring of decoded pictures waiting to be shown (the SDK sample's VoBuf);
   the vblank handler takes pictures out while the decode thread puts them in */
typedef struct {
    MovieOutPicture *data;          /* 0x00 */
    MovieOutTag *tag;               /* 0x04 */
    volatile s32 write;             /* 0x08 slot the next picture goes in */
    volatile s32 count;             /* 0x0C pictures waiting */
    s32 size;                       /* 0x10 slots */
} MovieOutBuffer;

/* the display the movie is shown on: two interlaced fields in GS memory, drawn from
   the picture uploaded as a texture behind them */
typedef struct {
    s64 pmode;                      /* 0x00 an sceGsDispEnv starts here */
    s64 smode2;                     /* 0x08 */
    s64 dispfb;                     /* 0x10 */
    s64 display;                    /* 0x18 */
    s64 bgcolor;                    /* 0x20 */
    s32 frame_base[2];              /* 0x28 frame buffer of each field, in GS pages */
    s32 screen_x;                   /* 0x30 where the picture goes on the screen */
    s32 screen_y;                   /* 0x34 */
    s32 width;                      /* 0x38 display size */
    s32 height;                     /* 0x3C */
    s32 picture_width;              /* 0x40 movie picture size */
    s32 picture_height;             /* 0x44 */
} MovieDisplay;

/* D_00437938: the INTC and DMAC handler ids of the movie player */
typedef struct {
    char pad00[0xB0];
    s32 unkB0;                      /* 0xB0 DMAC handler id */
    s32 unkB4;                      /* 0xB4 INTC handler id */
} IntcDmacState;

/* a time stamp queued with the stretch of stream data it belongs to */
typedef struct {
    s64 pts;                        /* 0x00 presentation time, -1 = none */
    s64 dts;                        /* 0x08 decoding time, -1 = none */
    s32 pos;                        /* 0x10 byte offset in the stream buffer */
    s32 len;                        /* 0x14 bytes covered */
} MovieTimeStamp;

/* the DMA and IPU registers saved while the stream DMA is stopped */
typedef struct {
    u32 d4_madr;                    /* 0x00 */
    u32 d4_tadr;                    /* 0x04 */
    u32 d4_qwc;                     /* 0x08 */
    u32 d4_chcr;                    /* 0x0C */
    u32 d3_madr;                    /* 0x10 */
    u32 d3_qwc;                     /* 0x14 */
    u32 d3_chcr;                    /* 0x18 */
    u32 ipu_bp;                     /* 0x1C */
    u32 ipu_ctrl;                   /* 0x20 */
} MovieDmaRegisters;

/* D_00437980: the ring of 0x800-byte blocks the video stream reaches the IPU through */
typedef struct {
    u8 *data;                       /* 0x00 the blocks */
    void *tags;                     /* 0x04 one DMA tag per block */
    s32 block_count;                /* 0x08 */
    s32 dma_start;                  /* 0x0C first block queued for DMA */
    s32 dma_count;                  /* 0x10 blocks queued for DMA */
    s32 read_bytes;                 /* 0x14 bytes written but not queued yet */
    s32 unk18;                      /* 0x18 */
    MovieDmaRegisters saved;        /* 0x1C */
    s32 sema;                       /* 0x40 */
    s32 is_active;                  /* 0x44 */
    s64 total_bytes;                /* 0x48 */
    MovieTimeStamp *stamps;         /* 0x50 ring of time stamps */
    s32 stamp_capacity;             /* 0x54 */
    s32 stamp_count;                /* 0x58 */
    s32 stamp_write;                /* 0x5C next slot written */
} MovieStreamBuffer;

/* `data` read as an integer, so that the compiler keeps the load behind a store through an
   `int *` (func_0021D0D0 only matches this way) */
#define MOVIE_STREAM_DATA_ADDRESS(stream) (*(s32 *)&(stream)->data)

/* D_00437938 seen whole: the libmpeg handle, its stream buffer and the decoder state */
typedef struct {
    char mpeg[0x48];                /* 0x00 an sceMpeg */
    MovieStreamBuffer stream;       /* 0x48 */
    s32 state;                      /* 0xA8 */
    s32 unkAC;                      /* 0xAC */
    s32 dmac_handler;               /* 0xB0 */
    s32 intc_handler;               /* 0xB4 */
} MovieVideoDecoder;

/* what libmpeg hands its time stamp callback (type 5), which fills in the two times */
typedef struct {
    s32 type;                       /* 0x00 */
    s64 pts;                        /* 0x08 */
    s64 dts;                        /* 0x10 */
} MovieStampCallbackData;

/* what libmpeg hands a stream callback: one stretch of an elementary stream, still in the
   read ring (the SDK's sceMpegCbDataStr) */
typedef struct {
    s32 type;                       /* 0x00 */
    u8 *header;                     /* 0x04 */
    u8 *data;                       /* 0x08 */
    u32 len;                        /* 0x0C */
    s64 pts;                        /* 0x10 */
    s64 dts;                        /* 0x18 */
} MovieStreamCallbackData;

#endif
