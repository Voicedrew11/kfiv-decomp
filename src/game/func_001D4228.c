#include "kf_game.h"
#include "kf_link.h"
#include "kf_load.h"

extern LinkSlotValues D_0043F720;
extern PoolPartSource *D_00420DF0[13];
extern int D_004472F0;
void func_002007C0(char *);
void func_002032E0(PoolPartSource *, u32);

/* Last stage of a node's load: points its slot at the two parts of the file, gives the buffer to
   the node it was loaded for and goes on to the next waiting node. */
void func_001D4228(LinkNode *node) {
    LinkSlotValues values = D_0043F720;
    LinkNode *target = node->unk10;
    LinkNode *loaded;
    LinkNode *next;
    LinkFile *file;
    void *pool_node;

    if ((u8)target->slot >= 13) {
        node->unk10 = NULL;
        node->unk20 = 1;
        return;
    }
    file = ((ChunkPoolNode *)node->pool_node)->buffer;
    D_00420DF0[target->slot]->texture = (char *)file + file->second_offset;
    D_00420DF0[target->slot]->file = (PoolPartFile *)((char *)file + file->first_offset);
    func_002007C0((char *)D_00420DF0[target->slot]->file);
    pool_node = node->unk10->pool_node;
    node->unk10->pool_node = node->pool_node;
    node->pool_node = pool_node;
    func_002032E0(D_00420DF0[node->unk10->slot], values.value[node->unk10->slot]);
    loaded = node->unk10;
    node->unk1C = 0;
    node->unk08 = -1;
    loaded->unk20 = 1;
    D_004472F0 = 0;
    next = loaded->next;
    if (next != NULL) {
        node->unk10 = next;
    } else {
        node->unk20 = 1;
        node->unk10 = NULL;
    }
    loaded->next = NULL;
}
