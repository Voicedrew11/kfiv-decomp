#include "kf_model.h"

extern ModelNode *D_004469D4;

/* Returns the node in use whose unk08 is object, or NULL. */
ModelNode *func_001C9580(int object) {
    ModelNode *node;

    for (node = D_004469D4; node != NULL; node = node->next) {
        if (node->unk08 == object) {
            return node;
        }
    }
    return NULL;
}
