#include "kf_load.h"

extern ChunkSource *D_004472B0;

/* Gives the upload's source node back to the free list. */
void func_001D45B8(ChunkUpload *upload) {
    ChunkSource *source = (ChunkSource *)upload->source;

    source->next = D_004472B0;
    D_004472B0 = source;
}
