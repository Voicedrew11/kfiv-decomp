#include "kf_movie.h"

int WaitSema(int);
int SignalSema(int);

/* Gives the one or two free stretches of the stream ring; two blocks stay unused (viBufBeginPut). */
void func_0021D0D0(MovieStreamBuffer *stream, u8 **first, int *first_size, u8 **second, int *second_size) {
    int write_pos;
    int until_end;
    int free_size;
    int reserved;

    WaitSema(stream->sema);
    reserved = stream->dma_count + 2;
    free_size = (stream->block_count - reserved) * 0x800 - stream->read_bytes;
    write_pos = ((stream->dma_start + stream->dma_count) * 0x800 + stream->read_bytes) % stream->unk18;
    until_end = stream->unk18 - write_pos;
    if (until_end >= free_size) {
        *first = (u8 *)(MOVIE_STREAM_DATA_ADDRESS(stream) + write_pos);
        *first_size = free_size;
        *second = NULL;
        *second_size = 0;
    } else {
        *first_size = until_end;
        *first = (u8 *)(MOVIE_STREAM_DATA_ADDRESS(stream) + write_pos);
        *second = (u8 *)MOVIE_STREAM_DATA_ADDRESS(stream);
        *second_size = free_size - (stream->unk18 - write_pos);
    }
    SignalSema(stream->sema);
}
