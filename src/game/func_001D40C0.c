#include "kf_link.h"
#include "kf_load.h"

extern ChunkUploadStages D_0043F710;

/* Runs the next stage of a node's upload until it is done. */
void func_001D40C0(LinkNode *node) {
    ChunkUploadStages stages = D_0043F710;

    if (node->unk20 == 0) {
        stages.stage[node->unk1C](node);
    }
}
