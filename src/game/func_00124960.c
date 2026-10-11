#include "kf_gs.h"
#include "kf_model.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E88(void);
void func_0010A008(void);
void func_0010AC80(int, s64, int, int);
void func_0010AEB8(void *model);
void func_0010CAE8(void *model, s64, s64, s64, s64);
void func_0010CB98(void *model);
void func_0010CF10(void *model);

/* Draw callback: sets the GS registers for the model's alpha and flags, then draws it. */
void func_00124960(ModelInstance *model) {
    if (model->unk102 & MODEL_FLAG_800) {
        func_0010CAE8(model, 0x220013540L, 0x60, -1, -1);
        func_0010CB98(model);
        func_0010AC80(0x3540, 0, 0x100, 0x100);
        func_0010A008();
        func_00109E88();
    } else {
        func_0010AEB8(model);
        func_0010CF10(model);
    }
    func_00109A50();

    if (128.0f <= model->alpha) {
        func_00109A80(GS_REG_ZBUF_1, 0xA000118L);
        func_00109A80(GS_REG_ZBUF_2, 0xA000118L);
        func_00109A80(GS_REG_TEST_1, 0x5000DL);
        if (model->unk102 & MODEL_FLAG_800) {
            func_00109A80(GS_REG_TEST_2, 0x30001L);
        } else {
            func_00109A80(GS_REG_TEST_2, 0x5000DL);
        }
    } else {
        func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
        func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
        func_00109A80(GS_REG_TEST_1, 0x5000DL);
        func_00109A80(GS_REG_TEST_2, 0x30001L);
    }
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(GS_REG_ALPHA_2, 0x9);
    func_00109AD0();
}
