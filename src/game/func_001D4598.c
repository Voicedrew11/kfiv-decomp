#include "kf_link.h"
#include "kf_load.h"

extern ChunkPoolNode *D_004472B0;

/* Takes a pool node off the free list for this link node, if one is left. */
int func_001D4598(LinkNode *node) { // match: declared int in kf_link.h, returns nothing
    ChunkPoolNode *free_node = D_004472B0;

    if (free_node != NULL) {
        node->pool_node = free_node;
        D_004472B0 = free_node->next;
    }
}
