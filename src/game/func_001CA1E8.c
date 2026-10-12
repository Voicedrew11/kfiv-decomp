#include "kf_model.h"

void func_00216C48(void *, void *, void *);
void func_00216778(void *, void *);
int func_0019B9F0(float, float *, int);
ModelNode *func_001CA520(ModelNode *);
extern int D_0038CC80[3];
extern ItemNode *D_004469D8;
extern f32 D_0044719C;
extern int D_004471A4;
extern int D_004471AC;
extern int D_004471B0;
extern f32 D_005B60C0[4];
extern f32 D_005B60D0[4];
extern f32 D_005B60F0[4];

/* Pickup step 4 (put back): moves the item back to where it lay and lets go of it. */
void func_001CA1E8(void) {
    MapObject *object;

    D_004471B0--;
    if (D_004471B0 > 0) {
        func_00216C48(&D_004469D8->model->position, &D_004469D8->model->position, D_005B60F0);
        func_0019B9F0(D_005B60D0[0], &D_004469D8->model->rotation.x, D_004471B0);
        func_0019B9F0(D_005B60D0[1], &D_004469D8->model->rotation.y, D_004471B0);
        func_0019B9F0(D_005B60D0[2], &D_004469D8->model->rotation.z, D_004471B0);
        D_0044719C = 1.0f - (f32)D_004471B0 / (f32)D_004471AC;
        return;
    }
    func_00216778(&D_004469D8->model->position, D_005B60C0);
    func_00216778(&D_004469D8->model->rotation, D_005B60D0);
    object = (MapObject *)D_004469D8->unk08;
    if (object != NULL && (object->unk10 & 0x10000) && object->unk04->kind == 3) {
        func_001CA520((ModelNode *)D_004469D8);
    }
    D_0038CC80[0] = 0;
    D_0044719C = 1.0f;
    D_004469D8 = NULL;
    D_004471A4 = 0;
}
