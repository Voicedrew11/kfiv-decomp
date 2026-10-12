#include "kf_entity.h"

extern LinkedPoint *D_004470D0;

/* Takes the first node off the free list, clearing its links; returns NULL when the list is empty. */
LinkedPoint *func_0019B260(void) {
    LinkedPoint *node;
    LinkedPoint *next;

    node = D_004470D0;
    if (node == NULL) {
        return NULL;
    }
    next = node->next;
    D_004470D0 = next;
    if (next != NULL) {
        next->prev = NULL;
    }
    node->prev = NULL;
    node->next = NULL;
    return node;
}
