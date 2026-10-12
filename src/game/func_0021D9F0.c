#include "kf_movie.h"

extern char D_00442140[];
extern char D_00442158[];

int func_0021D950(MovieStreamBuffer *, s32);
/* a debug print stub: takes the message the retail caller passes and ignores it */
void func_0021BDD8();

/* Shortens or drops the queued time stamps that the data of a new one overwrites (viBufModifyPts). */
int func_0021D9F0(MovieStreamBuffer *stream, MovieTimeStamp *new_stamp) {
    MovieTimeStamp *stamp;
    s32 read = (stream->stamp_write - stream->stamp_count + stream->stamp_capacity) % stream->stamp_capacity;
    s32 data_size = stream->block_count << 11;
    s32 keep_going = TRUE;

    if (stream->stamp_count > 0) {
        while (keep_going) {
            stamp = stream->stamps + read;
            if (stamp->len == 0 || new_stamp->len == 0) {
                break;
            }
            if (func_0021D950(stream, new_stamp->pos)) {
                if (stamp->pts >= 0) {
                    func_0021BDD8(D_00442140);
                    stamp->pts = -1;
                    stamp->dts = -1;
                    stamp->pos = 0;
                    stamp->len = 0;
                }
                stream->stamp_count = (stream->stamp_count - 1 >= 0) ? stream->stamp_count - 1 : 0;
            } else if ((stamp->pos + data_size - new_stamp->pos) % data_size < new_stamp->len) {
                s32 overlap = new_stamp->pos + new_stamp->len - stamp->pos;

                stamp->pos = new_stamp->pos + new_stamp->len;
                stamp->len -= overlap;
                if (stamp->len == 0) {
                    if (stamp->pts >= 0) {
                        func_0021BDD8(D_00442158);
                        stamp->pts = -1;
                        stamp->dts = -1;
                        stamp->pos = 0;
                        stamp->len = 0;
                    }
                    stream->stamp_count = (stream->stamp_count - 1 >= 0) ? stream->stamp_count - 1 : 0;
                }
            } else {
                keep_going = FALSE;
            }
            read = (read + 1) % stream->stamp_capacity;
        }
    }
    return 0;
}
