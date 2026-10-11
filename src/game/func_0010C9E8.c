#include "kf_model.h"

void func_00216778(void *, void *);
void func_00216BC0(void *, void *, void *);

/* Builds 32 transformed bone vectors from the actor source table. */
void func_0010C9E8(ModelInstance *actor) {
    float point[4];
    int i;
    BoneAnchor *anchor;
    Vector4 *position;

    anchor = actor->bone_anchors;
    position = actor->bone_positions;
    if (actor != NULL) {
        for (i = 0; i < 32; i++, anchor++, position++) {
            if (anchor->bone != 0xFF) {
                func_00216778(point, anchor);
                point[3] = 1.0f;
                func_00216BC0(position, &actor->parts[anchor->bone], point);
                func_00216BC0(position, actor->unk40, position);
            } else {
                func_00216778(position, &actor->position);
            }
        }
    }
}
