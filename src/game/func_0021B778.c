#include "kf_movie.h"
#include <libmpeg.h>

extern s32 D_00437820[];
extern MovieOutBuffer D_00437838;
extern MovieVideoDecoder D_00437938;
extern MovieBuffers D_004379F0;

void func_0021B9E0(void);
int func_0021B9E8(void);
int func_0021C138(MovieReadBuffer *, u8 **);
int func_0021C168(MovieReadBuffer *, int);
int func_0021C1B0(MovieReadBuffer *, u8 **);
int func_0021C1F8(MovieReadBuffer *, int);
int func_0021C448(MovieBuffers *);
int func_0021C5A8(MovieBuffers *);
s32 func_0021DFC0(char *);
int func_0021DFD0(MovieVideoDecoder *);
int func_0021E080(void *);
int func_0021E150(MovieVideoDecoder *);
s32 func_0021E588(char *);
void func_0021F7E8(s32);
void func_0021F830(void);
s32 func_0021FD68(MovieFile *, u32 *, s32, s32);

/* Plays the movie file to its end: reads it into the read ring, has libmpeg split it into the
   video and sound streams, starts picture and sound once both are buffered, then waits for the
   decoder to run dry. From the eleventh picture on, `skip_requested` can end it early
   (the SDK sample's readMpeg). */
int func_0021B778(MovieVideoDecoder *decoder, MovieReadBuffer *read_buffer, MovieFile *file,
                  int (*skip_requested)(void)) {
    u8 *put_dest;
    u8 *get_src;
    int put_size;
    int get_size;
    int read_rest = file->size;
    int write_rest = file->size;
    int count;
    int proceeded;
    int is_started = FALSE;
    int is_skipping = FALSE;

    /* the last 4 bytes are not played */
    while (write_rest > 4 && func_0021DFD0(decoder) != 3) {
        if (skip_requested != NULL && !is_skipping && ((sceMpeg *)decoder)->frameCount > 10) {
            is_skipping = skip_requested() != 0;
        }
        if (is_skipping && ((sceMpeg *)decoder)->frameCount > 10) {
            func_0021DFC0((char *)&D_00437938);
        }
        put_size = func_0021C138(read_buffer, &put_dest);
        if (read_rest > 0 && put_size >= 0x10000) {
            count = func_0021FD68(file, (u32 *)put_dest, 0x10000, file->lsn + (file->size - read_rest) / 0x800);
            func_0021C168(read_buffer, count);
            read_rest -= count;
        }
        func_0021B9E0();
        get_size = func_0021C1B0(read_buffer, &get_src);
        if (get_size > 0) {
            proceeded =
                sceMpegDemuxPssRing((sceMpeg *)decoder, get_src, get_size, (int)read_buffer, read_buffer->size);
            func_0021C1F8(read_buffer, proceeded);
            write_rest -= proceeded;
        }
        if (!is_started && func_0021E588((char *)&D_00437838) && func_0021B9E8()) {
            func_0021F7E8(1);
            if (D_00437820[0] != 0) {
                func_0021C448(&D_004379F0);
            }
            is_started = TRUE;
        }
    }
    while (!func_0021E080(decoder)) {
        func_0021B9E0();
    }
    while (!func_0021E150(decoder) && func_0021DFD0(decoder) != 3) {
        func_0021B9E0();
    }
    func_0021F830();
    if (D_00437820[0] != 0) {
        func_0021C5A8(&D_004379F0);
    }
    return TRUE;
}
