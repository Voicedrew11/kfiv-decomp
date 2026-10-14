#include "kf_model.h"

void Vector_Subtract(float *, float *, float *);
void Vector_Scale(float *, float *, float);
void Vector_Add(void *, void *, void *);

/* turns a packed vertex into one of floats */
#define TO_BLEND_VERTEX(to, from)                                                                  \
    (to).unk00[0] = (from)->x;                                                                     \
    (to).unk00[1] = (from)->y;                                                                     \
    (to).unk00[2] = (from)->z;                                                                     \
    (to).unk00[3] = 1.0f;                                                                          \
    (to).unk10[0] = (from)->unk08[0] * 4096.0f;                                                    \
    (to).unk10[1] = (from)->unk08[1] * 4096.0f;                                                    \
    (to).unk10[2] = (from)->unk08[2] * 4096.0f;                                                    \
    (to).unk10[3] = 0.0f;                                                                          \
    (to).unk20[0] = (from)->tex_u * 4096.0f;                                                           \
    (to).unk20[1] = (from)->tex_v * 4096.0f;                                                           \
    (to).unk20[2] = 1.0f;                                                                          \
    (to).unk20[3] = 0.0f;                                                                          \
    (to).unk30[0] = (from)->colour[0];                                                             \
    (to).unk30[1] = (from)->colour[1];                                                             \
    (to).unk30[2] = (from)->colour[2];                                                             \
    (to).unk30[3] = (from)->colour[3]

/* Writes steps + 1 float vertices that lead from one packed vertex to another in equal steps
   (func_00110D60 does the same between two float vertices). */
void func_001108E8(BlendVertex *out, ModelPartVertexWords *from, ModelPartVertexWords *last, int steps) {
    BlendVertex from_point;
    BlendVertex last_point;
    BlendVertex delta;
    BlendVertex *previous;
    f32 scale;
    int i;

    scale = 1.0f / steps;
    TO_BLEND_VERTEX(from_point, from);
    TO_BLEND_VERTEX(last_point, last);
    Vector_Subtract(delta.unk00, last_point.unk00, from_point.unk00);
    Vector_Scale(delta.unk00, delta.unk00, scale);
    Vector_Subtract(delta.unk10, last_point.unk10, from_point.unk10);
    Vector_Scale(delta.unk10, delta.unk10, scale);
    delta.unk20[0] = last_point.unk20[0] * from_point.unk20[0] * scale;
    delta.unk20[1] = last_point.unk20[1] * from_point.unk20[1] * scale;
    Vector_Subtract(delta.unk30, last_point.unk30, from_point.unk30);
    Vector_Scale(delta.unk30, delta.unk30, scale);
    previous = out;
    *out = from_point;
    out++;
    for (i = 1; i < steps; i++) {
        Vector_Add(out->unk00, previous->unk00, delta.unk00);
        out->unk00[3] = 1.0f;
        Vector_Add(out->unk10, previous->unk10, delta.unk10);
        *(s32 *)&out->unk10[3] = 0; // match: stored as an integer; as a float it would hold the loads below back
        out->unk20[0] = previous->unk20[0] + delta.unk20[0];
        out->unk20[1] = previous->unk20[1] + delta.unk20[1];
        out->unk20[2] = 1.0f;
        *(s32 *)&out->unk20[3] = 0; // match: an integer as well
        Vector_Add(out->unk30, previous->unk30, delta.unk30);
        previous++;
        out++;
    }
    *out = last_point;
}
