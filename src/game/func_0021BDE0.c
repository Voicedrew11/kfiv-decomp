#include "kf_movie.h"
#include <libmpeg.h>

extern MovieVideoDecoder D_00437938;
extern char D_004420E0[];

void func_0021BDD8(char *); // match: the empty stub is still passed the message
int func_0021BFD8(u8 *, int, u8 *, int, u8 *, int, u8 *, int);
void func_0021DF68(MovieVideoDecoder *, u8 **, s32 *, u8 **, s32 *);
void func_0021DF70(MovieVideoDecoder *, s32);
int func_0021DFE8(MovieVideoDecoder *, s64, s64, u8 *, s32);

/* libmpeg calls this with a stretch of the video stream: copies it out of the read ring into
   the stream ring and queues its time stamp (the SDK sample's videoCallback). */
int func_0021BDE0(sceMpeg *mpeg, MovieStreamCallbackData *stream_data, MovieReadBuffer *read_buffer) {
    u8 *first_src = stream_data->data;
    u8 *second_src = read_buffer->data;
    u32 first_src_size = read_buffer->data + read_buffer->size - stream_data->data;
    int second_src_size;
    u8 *first_dest;
    int first_dest_size;
    u8 *second_dest;
    int second_dest_size;
    int copied;

    if (stream_data->len < first_src_size) {
        first_src_size = stream_data->len;
    }
    second_src_size = stream_data->len - first_src_size;
    func_0021DF68(&D_00437938, &first_dest, &first_dest_size, &second_dest, &second_dest_size);
    copied = func_0021BFD8((u8 *)(((u32)first_dest & 0x0FFFFFFF) | 0x20000000), first_dest_size,
                           (u8 *)(((u32)second_dest & 0x0FFFFFFF) | 0x20000000), second_dest_size, first_src,
                           first_src_size, second_src, second_src_size);
    if (copied > 0) {
        if (!func_0021DFE8(&D_00437938, stream_data->pts, stream_data->dts, first_dest, copied)) {
            func_0021BDD8(D_004420E0);
        }
    }
    func_0021DF70(&D_00437938, copied);
    return copied > 0;
}
