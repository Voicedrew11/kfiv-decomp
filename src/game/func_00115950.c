#include "kf_screen.h"

extern AreaNode *D_00446788;
extern f32 D_00446768[]; /* the largest float */

#define USABLE(node) (!((node)->flags & 0x40000000))
#define IN_X(node) ((node)->left - margin <= point[0] && point[0] <= (node)->right + margin)
#define IN_Y(node) ((node)->unk44 - margin <= point[1] && point[1] <= (node)->unk54 + margin)
#define IN_Z(node) ((node)->top - margin <= point[2] && point[2] <= (node)->bottom + margin)

/* how far the point is outside the box along one axis */
#define DISTANCE(low, high, value)                                                                 \
    if ((value) < (low) - margin) {                                                                \
        distance = ((low) - margin) - (value);                                                     \
    } else {                                                                                       \
        distance = (value) - ((high) + margin);                                                    \
    }                                                                                              \
    if (distance < best_distance) {                                                                \
        best_distance = distance;                                                                  \
        best = node;                                                                               \
    }

/* Finds the box of the first list that holds the point (each side widened by margin). Failing
   that, the box nearest above or below it, then the nearest in z, then the nearest in x. */
AreaNode *func_00115950(f32 *point, f32 margin) {
    AreaNode *head = D_00446788;
    AreaNode *node;
    AreaNode *best;
    f32 best_distance;
    f32 distance;

    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Y(node) && IN_Z(node)) {
            return node;
        }
    }
    best_distance = D_00446768[0];
    best = NULL;
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Z(node)) {
            DISTANCE(node->unk44, node->unk54, point[1]);
        }
    }
    if (best != NULL) {
        return best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Y(node)) {
            DISTANCE(node->top, node->bottom, point[2]);
        }
    }
    if (best != NULL) {
        return best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_Y(node) && IN_Z(node)) {
            DISTANCE(node->left, node->right, point[0]);
        }
    }
    return best;
}
