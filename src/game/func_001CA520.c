#include "kf_model.h"

extern ModelNode *D_004469D4;

/* Unlinks node from the model node list and returns the node that followed it. */
ModelNode *func_001CA520(ModelNode *node) {
    ModelNode *next;

    next = node->next;
    if (node->prev != NULL) {
        node->prev->next = next;
    }
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (node == D_004469D4) {
        D_004469D4 = node->next;
    }
    node->prev = NULL;
    node->next = NULL;
    node->unk08 = 0;
    node->unk04 = 0;
    return next;
}
