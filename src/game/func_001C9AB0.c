#include "kf_model.h"

extern ModelNode *D_004469D4;
extern ModelNode *D_004469D8;
extern int D_00447198;
extern int D_004471A4;

/* Makes node the one being picked up, if none is yet and it is on the list of the ones in use. */
int func_001C9AB0(ModelNode *node) {
    if (node == NULL || D_004469D8 != NULL) {
        return FALSE;
    }
    if (D_004469D4 != node && node->prev == NULL && node->next == NULL) {
        return FALSE;
    }
    D_004469D8 = node;
    D_00447198 = 0;
    D_004471A4 = 0;
    return TRUE;
}
