#include "kf_movie.h"
#include <eeregs.h>

int WaitSema(int);
int SignalSema(int);

/* Takes the time stamp of the stream data the IPU is decoding now out of the queue (viBufGetTs). */
int func_0021DC90(MovieStreamBuffer *stream, MovieTimeStamp *stamp_out) {
    u32 d4_madr = *D4_MADR;
    u32 ipu_bp = *IPU_BP;
    s32 bit_position = stream->saved.ipu_bp & 0x7F;
    s32 fifo_pending = (ipu_bp >> 16) & 3;
    s32 fifo_count = (ipu_bp >> 8) & 0xF;
    u32 d4_madr_next = d4_madr - ((fifo_pending + fifo_count) << 4);
    s32 data_size = stream->block_count << 11;
    s32 is_end = FALSE;
    s32 skipped = 0;
    s32 stop;
    s32 stamp_count;
    s32 stamp_write;
    s32 index;

    WaitSema(stream->sema);
    stamp_out->pts = -1;
    stamp_out->dts = -1;
    stop = (d4_madr_next + (bit_position >> 3) + data_size - (u32)stream->data) % data_size;
    stamp_count = stream->stamp_count;
    stamp_write = stream->stamp_write;
    if (stamp_count > 0) {
        for (index = 0; index < stamp_count && !is_end; index++) {
            s32 read = (stamp_write - stamp_count + stream->stamp_capacity + index) % stream->stamp_capacity;

            if ((stop + data_size - stream->stamps[read].pos) % data_size < stream->stamps[read].len) {
                if (stream->stamps[read].pts >= 0) {
                    stamp_out->pts = stream->stamps[read].pts;
                    stamp_out->dts = stream->stamps[read].dts;
                    stream->stamps[read].pts = -1;
                    stream->stamps[read].dts = -1;
                }
                is_end = TRUE;
            } else {
                skipped++;
            }
        }
        stream->stamp_count = stamp_count - (stamp_count < skipped ? stamp_count : skipped);
    }
    SignalSema(stream->sema);
    return 1;
}
