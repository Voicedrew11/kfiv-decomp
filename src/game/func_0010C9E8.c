#include "kf_model.h"

void Vector_Copy(void *, void *);
void Vector_ApplyMatrix(void *, void *, void *);

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
                Vector_Copy(point, anchor);
                point[3] = 1.0f;
                Vector_ApplyMatrix(position, &actor->parts[anchor->bone], point);
                Vector_ApplyMatrix(position, actor->unk40, position);
            } else {
                Vector_Copy(position, &actor->position);
            }
        }
    }
}
