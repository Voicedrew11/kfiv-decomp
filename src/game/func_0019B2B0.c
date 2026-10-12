#include "kf_entity.h"

extern LinkedPoint *D_004470C8[2];

/* Unlinks a point from its active list (list_index picks the list) and returns the node that followed it. */
LinkedPoint *func_0019B2B0(LinkedPoint *node) {
    LinkedPoint *next;
    u8 list;

    next = node->next;
    list = node->list_index;
    if (node->prev != NULL) {
        node->prev->next = node->next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (D_004470C8[list] == node) {
        D_004470C8[list] = node->next;
    }
    node->prev = NULL;
    node->next = NULL;
    node->active = 0;
    return next;
}
