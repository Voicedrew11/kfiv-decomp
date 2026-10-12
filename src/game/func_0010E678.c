#include "kf_model.h"

/* Where bone `index` of a model is; the model's own position when it has no bones or there is no such bone. */
Vector4 *func_0010E678(ModelInstance *model, s32 index) {
    if (model->bone_anchors != NULL && index >= 0 && index < 32) {
        return &model->bone_positions[index];
    }
    return &model->position;
}
