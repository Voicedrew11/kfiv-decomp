#include "kf_model.h"
#include "kf_vu0.h"

/* Places the model's 32 bone points: each anchor's offset through its part's matrix and then the model's, or
   the model's position for an anchor without a bone. */
void func_0010E5A0(ModelInstance *model) {
    f32 point[4];
    int i;
    BoneAnchor *anchor;
    Vector4 *bone_position;

    if (model->bone_anchors != NULL) {
        anchor = model->bone_anchors;
        bone_position = model->bone_positions;
        for (i = 0; i < 32; i++, anchor++, bone_position++) {
            if (anchor->bone != 0xFF) {
                Vector_Copy(point, anchor);
                point[3] = 1.0f;
                Vector_ApplyMatrix(bone_position, model->parts[anchor->bone].unk00, point);
                Vector_ApplyMatrix(bone_position, model->unk40, bone_position);
            } else {
                Vector_Copy(bone_position, &model->position);
            }
        }
    }
}
