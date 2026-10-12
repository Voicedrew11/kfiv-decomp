#include "kf_movie.h"

/* Builds a source-chain DMA tag (scTag2). */
void func_0021CE20(MovieDmaTag *dma_tag, u32 address, u32 tag_id, u32 qwc) {
    dma_tag->tag = ((u64)address << 32) | ((u64)tag_id << 28) | (u64)qwc;
}
