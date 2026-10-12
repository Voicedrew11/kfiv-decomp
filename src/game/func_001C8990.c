#include "kf_model.h"

void func_00216C48(void *, void *, void *);
extern s32 D_00447190[2];
extern ItemNode *D_004469D4;
extern ItemNode D_0057E240[2][64];

/* Moves every item of a map by `offset`: the fixed items' definitions and models, and the
   dropped items lying about. */
void func_001C8990(int map, f32 *offset) {
    s32 i;
    ItemNode *node;

    for (i = 0; i < D_00447190[map]; i++) {
        if (D_0057E240[map][i].def != NULL) {
            func_00216C48(D_0057E240[map][i].def, D_0057E240[map][i].def, offset);
        }
        if (D_0057E240[map][i].model != NULL) {
            func_00216C48(&D_0057E240[map][i].model->position, &D_0057E240[map][i].model->position, offset);
        }
    }
    for (node = D_004469D4; node != NULL; node = node->next) {
        if ((node->flags & 1) == map && node->def == NULL) {
            // match: the same lines twice (one copy in the binary) give `offset` its saved register
            if (node->record != NULL) {
                func_00216C48(&node->model->position, &node->model->position, offset);
                node->unk28 += offset[1];
                node->unk2C += offset[1];
            } else if (node->unk08 != 0) {
                func_00216C48(&node->model->position, &node->model->position, offset);
                node->unk28 += offset[1];
                node->unk2C += offset[1];
            }
        }
    }
}
