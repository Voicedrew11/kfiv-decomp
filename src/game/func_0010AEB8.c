#include "kf_model.h"
#include "kf_texture.h"

extern int D_00447048;
extern int D_0044704C;
extern int D_00447050[2];
extern int D_00468190[];

void func_00109E88(void);
void func_0010B2E0(TextureTableHeader *, s32);
void func_00116B60(int *);

/* Send the textures a model needs that are not the ones already loaded. */
void func_0010AEB8(void *model_arg) {
    ModelInstance *model = model_arg;
    int changed;

    changed = FALSE;
    if (model->unk194 != 0) {
        if (D_00468190[(s16)model->unk190] != model->unk194 || model->unk178 != NULL) {
            switch ((s16)model->unk190) {
            case 0:
            case 1:
            case 2:
                D_00468190[(s16)model->unk190] = model->unk194;
                changed = TRUE;
                func_0010B2E0((TextureTableHeader *)model->unk194, (s32)model->unk178);
                break;
            case 3:
                func_00116B60((int *)model->unk194);
                break;
            }
        }
    }
    if (model->unk198 != 0) {
        if (D_00447050[model->unk192] != model->unk198 && model->unk192 == 0) {
            func_00116B60((int *)model->unk198);
        }
    }
    if (model->unk19C != 0 && D_00447048 != model->unk19C) {
        D_00447048 = model->unk19C;
        func_00116B60((int *)model->unk19C);
        changed = TRUE;
    }
    if (model->unk1A0 != 0 && D_0044704C != model->unk1A0) {
        D_0044704C = model->unk1A0;
        func_00116B60((int *)model->unk1A0);
        changed = TRUE;
    }
    if (changed) {
        if (model->unk1BC != NULL) {
            model->unk1BC(model);
        }
        func_00109E88();
    }
}
