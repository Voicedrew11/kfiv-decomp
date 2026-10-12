#include "kf_movie.h"

extern MovieStreamBuffer D_00437980;

int func_0021DBB8(MovieStreamBuffer *, MovieTimeStamp *);

/* Queues the time stamp of the stream data written at `start`. */
int func_0021DFE8(MovieVideoDecoder *decoder, s64 pts, s64 dts, u8 *start, s32 len) {
    MovieTimeStamp stamp;

    stamp.pts = pts;
    stamp.dts = dts;
    stamp.pos = start - decoder->stream.data;
    stamp.len = len;
    return func_0021DBB8(&D_00437980, &stamp); // match: the global buffer, not the decoder's own (the same one)
}
