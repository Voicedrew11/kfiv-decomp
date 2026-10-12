#include "kf_movie.h"
#include <eekernel.h>
#include <eeregs.h>

void func_0021CDB8(int);
void func_0021CE20(MovieDmaTag *, u32, u32, u32);

/* Sets up the stream ring over `block_count` 0x800-byte blocks and their DMA tags, and points
   DMA channel 4 (to the IPU) at it (viBufCreate, with viBufReset written out). */
int func_0021CE48(MovieStreamBuffer *stream, u8 *data, MovieDmaTag *tags, int block_count, MovieTimeStamp *stamps,
                  int stamp_capacity) {
    struct SemaParam sema_param;
    int i;

    stream->data = data;
    stream->tags = (void *)(((u32)tags & 0x0FFFFFFF) | 0x20000000);
    stream->block_count = block_count;
    stream->unk18 = block_count * 0x800;
    stream->dma_start = 0;
    stream->dma_count = 0;
    stream->read_bytes = 0;
    stream->is_active = TRUE;
    stream->total_bytes = 0;
    stream->stamps = stamps;
    stream->stamp_capacity = stamp_capacity;
    stream->stamp_count = 0;
    stream->stamp_write = 0;
    for (i = 0; i < stamp_capacity; i++) {
        stream->stamps[i].pts = -1;
        stream->stamps[i].dts = -1;
        stream->stamps[i].pos = 0;
        stream->stamps[i].len = 0;
    }
    sema_param.initCount = 1;
    sema_param.maxCount = 1;
    stream->sema = CreateSema(&sema_param);
    for (i = 0; i < stream->block_count; i++) {
        func_0021CE20(&tags[i], (u32)(data + i * 0x800), 3, 0x80);
    }
    func_0021CE20(&tags[i], (u32)tags & 0x0FFFFFFF, 2, 0);
    *D4_QWC = 0;
    *D4_MADR = (u32)stream->data & 0x0FFFFFFF;
    *D4_TADR = (u32)stream->tags & 0x0FFFFFFF;
    func_0021CDB8(5);
    return TRUE;
}
