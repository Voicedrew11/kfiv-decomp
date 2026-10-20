/* Movie playback (gfx/movie.c, FromSoftware's fslibmpg.c): libmpeg decodes the video stream through the IPU while
   the sound stream goes to the SPU2 on the IOP; built after the SDK's MPEG streaming sample. */
#ifndef KF_MOVIE_H
#define KF_MOVIE_H

#include "common.h"

/* the sound stream's state on its way to the IOP (the SDK sample's AudioDec, movie_audio) */
typedef struct {
    int state;                      /* 0x00 MOVIE_AUDIO_STATE_* */
    u8 header[0x28];                /* 0x04 the stream's "SShd" and "SSbd" headers */
    int header_count;               /* 0x2C header bytes received */
    u8 *data;                       /* 0x30 ring of PCM data on the EE */
    int put;                        /* 0x34 where the next data goes */
    int count;                      /* 0x38 bytes waiting */
    int size;                       /* 0x3C ring size */
    int total_bytes;                /* 0x40 */
    int iop_buffer;                 /* 0x44 IOP heap block the SPU2 plays from */
    int iop_buffer_size;            /* 0x48 */
    int iop_last_pos;               /* 0x4C where the next block goes in the IOP buffer */
    int iop_pause_pos;              /* 0x50 */
    int total_bytes_sent;           /* 0x54 */
    int iop_zero_buffer;            /* 0x58 IOP heap block of 0x800 zero bytes */
} MovieAudio;

#define MOVIE_AUDIO_STATE_INIT 0
#define MOVIE_AUDIO_STATE_PRESET 1
#define MOVIE_AUDIO_STATE_PLAY 2
#define MOVIE_AUDIO_STATE_PAUSE 3

/* four bytes copied whole, as MovieVideo_Flush copies the end code (byte aligned) */
typedef struct {
    u8 bytes[4];                    /* 0x00 */
} MovieEndCode;

/* a block of data handed to the IOP with sceSifSetDma (the SDK's sceSifDmaData) */
typedef struct {
    void *data;                     /* 0x00 */
    void *addr;                     /* 0x04 IOP address */
    int size;                       /* 0x08 */
    int attr;                       /* 0x0C */
} MovieSifDmaData;

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
    u64 pad08;                      /* 0x08 */
} MovieDmaTag;

/* the movie file being played: streamed from the disc when its path starts with "cdrom0:", read with sceRead
   otherwise */
typedef struct {
    s32 is_cd_stream;               /* 0x00 */
    s32 size;                       /* 0x04 bytes */
    u32 lsn;                        /* 0x08 an sceCdlFILE starts here: first sector */
    u32 cd_size;                    /* 0x0C bytes, from the disc directory */
    char pad10[0x1C];               /* 0x10 rest of the sceCdlFILE */
    s32 iop_buffer;                 /* 0x2C IOP heap block the stream uses as its ring buffer */
    s32 file_fd;                    /* 0x30 host file */
} MovieFile;

/* the display the movie is shown on: two interlaced fields in GS memory, drawn from the picture uploaded as a
   texture behind them */
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

/* movie_thread: the movie player's buffers, threads, file and display */
typedef struct {
    u32 heap_start;                 /* 0x00 the block Movie_Play is given to allocate from */
    u32 heap_next;                  /* 0x04 first free byte (Movie_HeapAlloc) */
    u32 heap_end;                   /* 0x08 last byte */
    char pad0C[0x04];
    void *idle_stack;               /* 0x10 0x800 bytes */
    void *decode_stack;             /* 0x14 0x4000 bytes */
    u8 *mpeg_work;                  /* 0x18 0x13B800 bytes, libmpeg's work area */
    u8 *audio_data;                 /* 0x1C 0xC000 bytes, MovieAudio.data */
    u8 *stream_data;                /* 0x20 0x40000 bytes, the 0x80 blocks of MovieStreamBuffer */
    MovieDmaTag *stream_tags;       /* 0x24 0x810 bytes, their DMA tags */
    void *out_pictures;             /* 0x28 five MovieOutPicture */
    void *out_tags;                 /* 0x2C five MovieOutTag */
    void *time_stamps;              /* 0x30 0x3C00 bytes, 0x280 MovieTimeStamp */
    void *spare_buffer;             /* 0x34 0x100-byte buffer allocated, never used */
    s32 read_buffer;                /* 0x38 the MovieReadBuffer (MOVIE_READ_BUFFER) */
    char pad3C[0x04];
    s32 decode_thread;              /* 0x40 thread id */
    s32 idle_thread;                /* 0x44 thread id */
    char pad48[0x04];
    MovieFile file;                 /* 0x4C */
    MovieDisplay display;           /* 0x80 */
    char padC8[0x08];
    u32 volume;                     /* 0xD0 master volume of the sound, at most 0x3FFF */
    s16 video_mode;                 /* 0xD4 sceGsResetGraph's mode */
    s16 display_x;                  /* 0xD6 sceGsSetDefDispEnv's offset */
    s16 display_y;                  /* 0xD8 */
} MovieThreadState;

/* MovieThreadState.read_buffer as the pointer it holds: Movie_Setup only matches when the load has a pointer
   type (it then moves ahead of the stores to the DMA registers) */
typedef struct {
    char pad00[0x38];
    MovieReadBuffer *read_buffer;   /* 0x38 */
} MovieThreadStatePointers;

#define MOVIE_READ_BUFFER(state) (((MovieThreadStatePointers *)(state))->read_buffer)

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

#define MOVIE_OUT_STATUS_EMPTY 0
#define MOVIE_OUT_STATUS_SHOWN_EVEN 1   /* the even field has been sent, the odd one is next */
#define MOVIE_OUT_STATUS_FULL 2

/* movie_out_buffer: ring of decoded pictures waiting to be shown (the SDK sample's VoBuf); the vblank handler takes
   pictures out while the decode thread puts them in */
typedef struct {
    MovieOutPicture *data;          /* 0x00 */
    MovieOutTag *tag;               /* 0x04 */
    volatile s32 write;             /* 0x08 slot the next picture goes in */
    volatile s32 count;             /* 0x0C pictures waiting */
    s32 size;                       /* 0x10 slots */
} MovieOutBuffer;

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

/* movie_decoder.stream: the ring of 0x800-byte blocks the video stream reaches the IPU through (the SDK sample's ViBuf) */
typedef struct {
    u8 *data;                       /* 0x00 the blocks */
    void *tags;                     /* 0x04 one DMA tag per block */
    s32 block_count;                /* 0x08 */
    s32 dma_start;                  /* 0x0C first block queued for DMA */
    s32 dma_count;                  /* 0x10 blocks queued for DMA */
    s32 read_bytes;                 /* 0x14 bytes written but not queued yet */
    s32 data_size;                  /* 0x18 block_count * 0x800 */
    MovieDmaRegisters saved;        /* 0x1C */
    s32 sema;                       /* 0x40 */
    s32 is_active;                  /* 0x44 */
    s64 total_bytes;                /* 0x48 */
    MovieTimeStamp *stamps;         /* 0x50 ring of time stamps */
    s32 stamp_capacity;             /* 0x54 */
    s32 stamp_count;                /* 0x58 */
    s32 stamp_write;                /* 0x5C next slot written */
} MovieStreamBuffer;

/* `data` read as an integer, so that the compiler keeps the load behind a store through an `int *`
   (MovieStream_BeginPut only matches this way) */
#define MOVIE_STREAM_DATA_ADDRESS(stream) (*(s32 *)&(stream)->data)

/* movie_decoder: the libmpeg handle, its stream buffer and the decoder state (the SDK sample's VideoDec) */
typedef struct {
    char mpeg[0x48];                /* 0x00 an sceMpeg */
    MovieStreamBuffer stream;       /* 0x48 */
    s32 state;                      /* 0xA8 MOVIE_VIDEO_STATE_* */
    s32 unkAC;                      /* 0xAC */
    s32 dmac_handler;               /* 0xB0 */
    s32 intc_handler;               /* 0xB4 */
} MovieVideoDecoder;

#define MOVIE_VIDEO_STATE_RUNNING 0
#define MOVIE_VIDEO_STATE_ABORT 1   /* skipping: the decode loop stops */
#define MOVIE_VIDEO_STATE_FLUSHED 2 /* the end of the stream was given */
#define MOVIE_VIDEO_STATE_ENDED 3   /* the decode thread is done */

/* what libmpeg hands its time stamp callback (type 5), which fills in the two times */
typedef struct {
    s32 type;                       /* 0x00 */
    s64 pts;                        /* 0x08 */
    s64 dts;                        /* 0x10 */
} MovieStampCallbackData;

/* what libmpeg hands a stream callback: one stretch of an elementary stream, still in the read ring (the SDK's
   sceMpegCbDataStr) */
typedef struct {
    s32 type;                       /* 0x00 */
    u8 *header;                     /* 0x04 */
    u8 *data;                       /* 0x08 */
    u32 len;                        /* 0x0C */
    s64 pts;                        /* 0x10 */
    s64 dts;                        /* 0x18 */
} MovieStreamCallbackData;

/* Plays a movie file to its end (or until skip_requested, polled from the eleventh picture on, returns
   nonzero), with the player's buffers carved out of the heap_size bytes at heap. volume is the sound's master
   volume, video_mode, display_x and display_y set up the display. Returns -1 when the block is too small. */
int Movie_Play(char *path, u32 heap, u32 heap_size, int (*skip_requested)(void), u32 volume, s16 video_mode,
               s16 display_x, s16 display_y);

#endif
