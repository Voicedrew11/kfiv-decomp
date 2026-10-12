#include "kf_model.h"

extern ModelNode *D_004469D4;

/* Puts a node at the head of the list at D_004469D4. */
void func_001CA500(ModelNode *node) {
    node->prev = NULL;
    node->next = D_004469D4;
    if (D_004469D4 != NULL) {
        D_004469D4->prev = node;
    }
    D_004469D4 = node;
}
