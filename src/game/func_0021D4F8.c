#include "kf_movie.h"
#include <eeregs.h>

/* whether block `index` is one of `count` blocks from `start` in a ring of `size` */
#define IS_IN_REGION(index, start, count, size)     (((index) + (size) - (start)) % (size) >= 0 && ((index) + (size) - (start)) % (size) < (count))

int WaitSema(int);
int SignalSema(int);
int func_0021CD10(MovieStreamBuffer *, u32);
void func_0021CD50(int);
void func_0021CDB8(int);

/* Restarts the stream DMA and the IPU from the registers saved when they were stopped (viBufRestartDMA). */
int func_0021D4F8(MovieStreamBuffer *stream) {
    s32 bit_position = stream->saved.ipu_bp & 0x7F;
    s32 fifo_pending = (stream->saved.ipu_bp >> 16) & 3;
    s32 fifo_count = (stream->saved.ipu_bp >> 8) & 0xF;
    u32 d4_madr_next = stream->saved.d4_madr - ((fifo_pending + fifo_count) << 4);
    u32 d4_qwc_next = stream->saved.d4_qwc + (fifo_pending + fifo_count);
    u32 d4_tadr_next = stream->saved.d4_tadr;
    u32 d4_chcr_next = stream->saved.d4_chcr | 0x100;
    s32 tag_id;
    s32 index;
    s32 index_next;

    WaitSema(stream->sema);
    if (d4_madr_next < (u32)stream->data) {
        d4_qwc_next = ((u32)stream->data - d4_madr_next) >> 4;
        d4_madr_next += stream->block_count << 11;
        d4_tadr_next = (u32)stream->tags & 0x0FFFFFFF;
        tag_id = (stream->saved.d4_madr == (u32)stream->data ||
                  stream->saved.d4_madr == (u32)stream->data + (stream->block_count << 11))
                     ? 0
                     : 3;
        d4_chcr_next = (stream->saved.d4_chcr & 0x0FFFFFFF) | (tag_id << 28) | 0x100;
        if (!IS_IN_REGION(0, stream->dma_start, stream->dma_count, stream->block_count)) {
            stream->dma_start = stream->block_count - 1;
            stream->dma_count++;
        }
    } else {
        index = func_0021CD10(stream, stream->saved.d4_madr);
        index_next = func_0021CD10(stream, d4_madr_next);
        if (index != index_next) {
            d4_qwc_next = ((u32)stream->data + (index << 11) - d4_madr_next) >> 4;
            d4_tadr_next = ((index << 4) + (u32)stream->tags) & 0x0FFFFFFF;
            tag_id = ((u32)stream->data + (stream->saved.d4_madr - (u32)stream->data) % (stream->block_count << 11) ==
                      (u32)stream->data + (((stream->dma_start + stream->dma_count) % stream->block_count) << 11))
                         ? 0
                         : 3;
            d4_chcr_next = (stream->saved.d4_chcr & 0x0FFFFFFF) | (tag_id << 28) | 0x100;
            if (!IS_IN_REGION(index_next, stream->dma_start, stream->dma_count, stream->block_count)) {
                stream->dma_start = index_next;
                stream->dma_count++;
            }
        }
    }
    if (stream->saved.d3_madr != 0 && stream->saved.d3_qwc != 0) {
        *D3_MADR = stream->saved.d3_madr;
        *D3_QWC = stream->saved.d3_qwc;
        func_0021CD50(stream->saved.d3_chcr | 0x100);
    }
    if (stream->dma_count != 0) {
        while (*IPU_CTRL < 0) {
        }
        *IPU_CMD = bit_position;
        while (*IPU_CTRL < 0) {
        }
    }
    *D4_MADR = d4_madr_next;
    *D4_TADR = d4_tadr_next;
    *D4_QWC = d4_qwc_next;
    if (stream->dma_count != 0) {
        func_0021CDB8(d4_chcr_next);
    }
    *IPU_CTRL = stream->saved.ipu_ctrl;
    stream->is_active = TRUE;
    SignalSema(stream->sema);
    return 1;
}
