#include "kf_model.h"

/* a float to the nearest whole number, halves away from 0 */
#define ROUND_HALF(value) ((value) > 0.0f ? (value) + 0.5f : (value) - 0.5f)

/* Writes steps + 1 vertices that lead from one vertex to another in equal steps; the position is
   worked out afresh for each as floats and rounded, the rest is stepped as whole numbers. */
void func_0010FC30(ModelPartVertexWords *out, ModelPartVertexWords *from, ModelPartVertexWords *last, int steps) {
    ModelPartVertexWords step;
    f32 position[4];
    f32 delta[4];
    f32 current[4];
    ModelPartVertexWords *previous;
    int i;

    position[0] = from->x;
    position[1] = from->y;
    position[2] = from->z;
    position[3] = 1.0f;
    delta[0] = last->x - position[0];
    delta[1] = last->y - position[1];
    delta[2] = last->z - position[2];
    delta[3] = 0.0f;
    step.unk08[0] = (last->unk08[0] - from->unk08[0]) / steps;
    step.unk08[1] = (last->unk08[1] - from->unk08[1]) / steps;
    step.unk08[2] = (last->unk08[2] - from->unk08[2]) / steps;
    step.u = (last->u - from->u) / steps;
    step.v = (last->v - from->v) / steps;
    step.colour[0] = (last->colour[0] - from->colour[0]) / steps;
    step.colour[1] = (last->colour[1] - from->colour[1]) / steps;
    step.colour[2] = (last->colour[2] - from->colour[2]) / steps;
    step.colour[3] = (last->colour[3] - from->colour[3]) / steps;
    previous = out;
    *out = *from;
    out->unk14 = 0x1000;
    out->unk0E = 0;
    out->unk16 = 0;
    out++;
    for (i = 1; i < steps; i++) {
        current[0] = position[0] + delta[0] * i / steps;
        current[1] = position[1] + delta[1] * i / steps;
        current[2] = position[2] + delta[2] * i / steps;
        out->x = ROUND_HALF(current[0]);
        out->y = ROUND_HALF(current[1]);
        out->z = ROUND_HALF(current[2]);
        out->unk06 = 1;
        out->unk08[0] = previous->unk08[0] + step.unk08[0];
        out->unk08[1] = previous->unk08[1] + step.unk08[1];
        out->unk08[2] = previous->unk08[2] + step.unk08[2];
        out->unk0E = 0;
        out->u = previous->u + step.u;
        out->v = previous->v + step.v;
        out->unk14 = 0x1000;
        out->unk16 = 0x7FFF;
        out->colour[0] = previous->colour[0] + step.colour[0];
        out->colour[1] = previous->colour[1] + step.colour[1];
        out->colour[2] = previous->colour[2] + step.colour[2];
        out->colour[3] = previous->colour[3] + step.colour[3];
        previous++;
        out++;
    }
    *out = *last;
    out->unk14 = 0x1000;
    out->unk0E = 0;
    out->unk16 = 0;
}
