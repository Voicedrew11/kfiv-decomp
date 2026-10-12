#include "kf_map.h"

void func_00216C48(void *, void *, void *);
void func_001BC1C0(int);
void func_0011E608(char *);
void *memset(void *, int, u32);
ModelNode *func_001CA520(ModelNode *);
extern char D_00356928[0x80][0x40];
extern MapSlot D_0038CD20[];
extern int D_0038CC80[3];
extern ItemNode *D_004469D8;
extern f32 D_0044719C;
extern int D_004471A4;
extern int D_004471B0;
extern f32 D_005B60F0[4];

/* Pickup step 3: moves the item to the player, then hands it over and frees its node. */
void func_001CA0E0(void) {
    ItemNode *node = D_004469D8;

    D_004471B0--;
    if (D_004471B0 > 0) {
        func_00216C48(&node->model->position, &node->model->position, D_005B60F0);
        return;
    }
    func_001BC1C0((s16)node->item);
    if (node->def != NULL && node->def->required_flag != 0) {
        D_00356928[D_0038CD20[node->flags & 1].tag][node->index]++;
    } else if (node->unk08 != 0) {
        func_0011E608((char *)node->unk08);
    } else if (node->record != NULL) {
        memset(node->record, 0, sizeof(DroppedItemRecord));
    }
    func_001CA520((ModelNode *)D_004469D8);
    D_0038CC80[0] = 0;
    D_0044719C = 1.0f;
    D_004469D8 = NULL;
    D_004471A4 = 0;
}
