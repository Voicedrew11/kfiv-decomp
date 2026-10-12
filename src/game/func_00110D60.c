#include "kf_model.h"

void KF_Vector_Subtract(float *, float *, float *);
void KF_Vector_Scale(float *, float *, float);
void func_00216C48(void *, void *, void *);

/* Writes steps + 1 vertices that lead from one vertex to another in equal steps
   (func_001108E8 does the same between two packed vertices). */
void func_00110D60(BlendVertex *out, BlendVertex *from, BlendVertex *last, int steps) {
    BlendVertex delta;
    BlendVertex *previous;
    f32 scale;
    int i;

    scale = 1.0f / steps;
    KF_Vector_Subtract(delta.unk00, last->unk00, from->unk00);
    KF_Vector_Scale(delta.unk00, delta.unk00, scale);
    KF_Vector_Subtract(delta.unk10, last->unk10, from->unk10);
    KF_Vector_Scale(delta.unk10, delta.unk10, scale);
    // match: retail bug, a product where every other component uses the difference last - from
    delta.unk20[0] = last->unk20[0] * from->unk20[0] * scale;
    delta.unk20[1] = last->unk20[1] * from->unk20[1] * scale; // match: retail bug, as above
    KF_Vector_Subtract(delta.unk30, last->unk30, from->unk30);
    KF_Vector_Scale(delta.unk30, delta.unk30, scale);
    previous = out;
    *out = *from;
    out++;
    for (i = 1; i < steps; i++) {
        func_00216C48(out->unk00, previous->unk00, delta.unk00);
        out->unk00[3] = 1.0f;
        func_00216C48(out->unk10, previous->unk10, delta.unk10);
        *(s32 *)&out->unk10[3] = 0; // match: stored as an integer; as a float it would hold the loads below back
        out->unk20[0] = previous->unk20[0] + delta.unk20[0];
        out->unk20[1] = previous->unk20[1] + delta.unk20[1];
        out->unk20[2] = 1.0f;
        *(s32 *)&out->unk20[3] = 0; // match: an integer as well
        func_00216C48(out->unk30, previous->unk30, delta.unk30);
        previous++;
        out++;
    }
    *out = *last;
}
