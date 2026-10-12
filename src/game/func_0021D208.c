#include "kf_movie.h"
#include <eeregs.h>

extern char D_00442128[];

int WaitSema(int);
int SignalSema(int);
void func_0021BDD8(char *); // match: the empty stub is still passed the message
void func_0021CDB8(int);
int func_0021CD10(MovieStreamBuffer *, u32);
void func_0021CE20(MovieDmaTag *, u32, u32, u32);

/* Queues the whole blocks written since the last call for DMA to the IPU and restarts the
   channel (viBufAddDMA). */
int func_0021D208(MovieStreamBuffer *stream) {
    int i;
    int index;
    int tag_id;
    int last;
    u32 d4_chcr;
    int is_new_data = FALSE;
    int consumed;
    int read_start;
    int read_count;

    WaitSema(stream->sema);
    if (!stream->is_active) {
        func_0021BDD8(D_00442128);
        return FALSE; // match: leaves without SignalSema, as the SDK sample does
    }
    /* stop the channel; D4_CHCR then holds the tag it stopped on */
    func_0021CDB8(5);
    d4_chcr = *D4_CHCR;
    index = func_0021CD10(stream, *D4_MADR);
    consumed = (index + stream->block_count - stream->dma_start) % stream->block_count;
    stream->dma_start = (stream->dma_start + consumed) % stream->block_count;
    stream->dma_count -= consumed;
    read_start = (stream->dma_start + stream->dma_count) % stream->block_count;
    read_count = stream->read_bytes / 0x800;
    stream->read_bytes %= 0x800;
    if (read_count > 0) {
        /* the tag that ended the chain becomes an ordinary one */
        last = (stream->dma_start + stream->dma_count - 1 + stream->block_count) % stream->block_count;
        func_0021CE20((MovieDmaTag *)stream->tags + last, (u32)(stream->data + last * 0x800), 3, 0x80);
        is_new_data = TRUE;
    }
    index = read_start;
    for (i = 0; i < read_count; i++) {
        tag_id = (i == read_count - 1) ? 0 : 3;
        func_0021CE20((MovieDmaTag *)stream->tags + index, (u32)(stream->data + index * 0x800), tag_id, 0x80);
        index = (index + 1) % stream->block_count;
    }
    stream->dma_count += read_count;
    if (stream->dma_count != 0) {
        if (is_new_data) {
            d4_chcr = (d4_chcr & 0x0FFFFFFF) | 0x30000000;
        }
        func_0021CDB8(d4_chcr | 0x100);
    }
    SignalSema(stream->sema);
    return TRUE;
}
