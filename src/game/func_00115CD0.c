#include "kf_screen.h"
#include "kf_map.h"
#include "kf_game.h"

extern AreaNode *D_00446788;
extern f32 D_0044676C[]; /* the largest float */

#define USABLE(node) (((u8)(AREA_NODE_WORD(node) >> 27) & 1) == map && !((node)->flags & 0x40000000))
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

/* Finds the box of the first list that belongs to the map slot and holds the point (each side widened by margin). Failing
   that, the box nearest above or below it, then the nearest in z, then the nearest in x. */
MapRecordInfo *func_00115CD0(CameraState *position, f32 margin, int map) {
    f32 *point = (f32 *)position;
    AreaNode *head = D_00446788;
    AreaNode *node;
    AreaNode *best;
    f32 best_distance;
    f32 distance;

    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Y(node) && IN_Z(node)) {
            return (MapRecordInfo *)node;
        }
    }
    best_distance = D_0044676C[0];
    best = NULL;
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Z(node)) {
            DISTANCE(node->unk44, node->unk54, point[1]);
        }
    }
    if (best != NULL) {
        return (MapRecordInfo *)best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_X(node) && IN_Y(node)) {
            DISTANCE(node->top, node->bottom, point[2]);
        }
    }
    if (best != NULL) {
        return (MapRecordInfo *)best;
    }
    for (node = head; node != NULL; node = node->next) {
        if (USABLE(node) && IN_Y(node) && IN_Z(node)) {
            DISTANCE(node->left, node->right, point[0]);
        }
    }
    return (MapRecordInfo *)best;
}
