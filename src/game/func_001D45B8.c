#include "kf_load.h"

extern ChunkPoolNode *D_004472B0;

/* Gives the upload's pool node back to the free list. */
void func_001D45B8(ChunkUpload *upload) {
    ChunkPoolNode *pool_node = upload->pool_node;

    pool_node->next = D_004472B0;
    D_004472B0 = pool_node;
}
