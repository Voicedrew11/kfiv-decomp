#include "kf_movie.h"

/* Block index of a DMA address in the stream ring; the tag after the last block counts as 0 (getFIFOindex). */
int func_0021CD10(MovieStreamBuffer *stream, u32 address) {
    if (address == ((u32)(stream->block_count * sizeof(MovieDmaTag) + (u32)stream->tags + sizeof(MovieDmaTag)) & 0x0FFFFFFF)) {
        return 0;
    }
    return (address - (u32)stream->data) / 0x800;
}
