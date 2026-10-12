#include "kf_movie.h"

/* Whether one of the queued time stamps covers the stream position `stop`. */
int func_0021D950(MovieStreamBuffer *stream, s32 stop) {
    s32 data_size = stream->block_count << 11;
    s32 is_end = FALSE;
    s32 stamp_count = stream->stamp_count;
    s32 found = FALSE;
    s32 stamp_write = stream->stamp_write;
    s32 index;

    if (stamp_count > 0) {
        for (index = 0; index < stamp_count && !is_end; index++) {
            s32 read = (stamp_write - stamp_count + stream->stamp_capacity + index) % stream->stamp_capacity;

            if ((stop + data_size - stream->stamps[read].pos) % data_size < stream->stamps[read].len) {
                is_end = TRUE;
                found = TRUE;
            }
        }
    }
    return found;
}
