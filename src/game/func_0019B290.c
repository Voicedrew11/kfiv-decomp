#include "common.h"

// match: EntityPoolNode is not in a shared header yet (see the report)
typedef struct EntityPoolNode {
    char pad00[0x38];
    struct EntityPoolNode *prev;
    struct EntityPoolNode *next;
} EntityPoolNode;

extern EntityPoolNode *D_004470D0;

/* Pushes a node onto the front of the list at D_004470D0. */
void func_0019B290(EntityPoolNode *node) {
    node->prev = NULL;
    node->next = D_004470D0;
    if (D_004470D0 != NULL) {
        D_004470D0->prev = node;
    }
    D_004470D0 = node;
}
