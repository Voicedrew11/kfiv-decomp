#include "kf_screen.h"

/* Tells whether the point lies inside the node's box grown by margin on every side. */
int func_001160B0(f32 *point, AreaNode *node, f32 margin) {
    int inside;

    if (node != NULL) {
        inside = FALSE;
        if (!(node->flags & 0x40000000) && node->left - margin <= point[0] && point[0] <= node->right + margin &&
            node->unk44 - margin <= point[1] && point[1] <= node->unk54 + margin &&
            node->top - margin <= point[2] && point[2] <= node->bottom + margin) {
            inside = TRUE;
        }
        return inside;
    }
    return FALSE;
}
