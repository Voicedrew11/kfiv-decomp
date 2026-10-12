#include "kf_model.h"

// match: ModelPart's byte at 0x6C is not named in kf_model.h yet (add u8 unk6C there)
typedef struct {
    char pad00[0x6C];
    u8 unk6C;
    char pad6D[0x13];
} ModelPartUnk6C;

void func_0019A430(ModelInstance *model, int index, u8 value) {
    ((ModelPartUnk6C *)model->parts)[index].unk6C = value;
}
