#include "model/model.h"
#include "gfx/gs.h"
#include <eeregs.h>

/* which sides of the screen a point is beyond, the bits of CLIP_SIDES */
#define SCREEN_SIDE_LEFT 1
#define SCREEN_SIDE_RIGHT 2
#define SCREEN_SIDE_TOP 4
#define SCREEN_SIDE_BOTTOM 8

#define CLIP_SIDES(point, sides)                                                                   \
    if ((point)->x < 0x6C00) {                                                                   \
        (sides) |= SCREEN_SIDE_LEFT;                                                                              \
    }                                                                                              \
    if ((point)->x > 0x9400) {                                                                   \
        (sides) |= SCREEN_SIDE_RIGHT;                                                                              \
    }                                                                                              \
    if ((point)->y < 0x7200) {                                                                   \
        (sides) |= SCREEN_SIDE_TOP;                                                                              \
    }                                                                                              \
    if ((point)->y > 0x8E00) {                                                                   \
        (sides) |= SCREEN_SIDE_BOTTOM;                                                                              \
    }
/* whether a point is too deep or outside the range the GS can draw */
#define OUT_OF_RANGE(point) ((point)->z > 0xFFFF0 || (((point)->x | (point)->y) & SCREEN_XY_OUTSIDE_MASK))
typedef u32 u_long128 __attribute__((mode(TI)));
/* copies a quadword and steps both pointers past it */
#define COPY_QWORD(to, from)                                                                       \
    *(u_long128 *)(to) = *(u_long128 *)(from);                                                               \
    (from) += 4;                                                                                   \
    (to) += 4
/* a float to the nearest whole number, halves away from 0 */
#define ROUND_HALF(value) ((value) > 0.0f ? (value) + 0.5f : (value) - 0.5f)
/* turns a packed vertex into one of floats */
#define TO_BLEND_VERTEX(to, from)                                                                  \
    (to).position[0] = (from)->x;                                                                     \
    (to).position[1] = (from)->y;                                                                     \
    (to).position[2] = (from)->z;                                                                     \
    (to).position[3] = 1.0f;                                                                          \
    (to).normal[0] = (from)->normal[0] * 4096.0f;                                                    \
    (to).normal[1] = (from)->normal[1] * 4096.0f;                                                    \
    (to).normal[2] = (from)->normal[2] * 4096.0f;                                                    \
    (to).normal[3] = 0.0f;                                                                          \
    (to).tex_coords[0] = (from)->tex_u * 4096.0f;                                                           \
    (to).tex_coords[1] = (from)->tex_v * 4096.0f;                                                           \
    (to).tex_coords[2] = 1.0f;                                                                          \
    (to).tex_coords[3] = 0.0f;                                                                          \
    (to).colour[0] = (from)->colour[0];                                                             \
    (to).colour[1] = (from)->colour[1];                                                             \
    (to).colour[2] = (from)->colour[2];                                                             \
    (to).colour[3] = (from)->colour[3]
void sceVu0MulMatrix(void *, void *, void *);
void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);
void Vu0_ProjectToScreen3(int *, int *, int *, f32 *, f32 *, f32 *);
void ModelScreen_QueueCutTriangle(u32 *, ModelPartVertexWords *, int, int, void *);
u32 *Packet_Alloc(int);
void Packet_AppendToChain(u32 *, u32 *);
void ModelScreen_StepVertices(ModelPartVertexWords *, ModelPartVertexWords *, ModelPartVertexWords *, int);
void Vector_Add(void *, void *, void *);
void ModelScreen_StepFloatVertices(BlendVertex *, BlendVertex *, BlendVertex *, int);
void Vector_Subtract(float *, float *, float *);
void Vector_Scale(float *, float *, float);
extern f32 world_screen_matrix[16];      /* world to screen */
extern int cut_triangle_count;  /* triangles cut up this way */

double sin(double);
int FlushCache(int);
extern int screen_uv_frame;         /* 0..59, stepped by ModelScreen_SetUvsFromScreen */
extern int screen_uv_frame_copy;    /* the same value; nothing reads it */

/* Looks for the triangles of a model that the GS could not draw as they are (a corner out of its
   range) but that are partly on the screen, and queues each cut up into smaller ones. */
void ModelScreen_CutClippedTriangles(ModelInstance *model) {
    f32 local_matrix[16];
    f32 screen_matrix[16];
    f32 point0[4];
    f32 point1[4];
    f32 point2[4];
    s32 clip[3];
    u16 length; // match: declared before i, the spill slots of length + 1, i and j follow this order
    s32 i = 0;
    s32 part_count = model->part_count;
    ModelPart *part = model->parts;

    for (; i < part_count; i++, part++) {
        s32 j;
        s32 count;
        s32 first = TRUE;
        ModelPartData *block;

        sceVu0MulMatrix(local_matrix, model->model_matrix, part);
        sceVu0MulMatrix(screen_matrix, world_screen_matrix, local_matrix);
        Vu0_LoadMatrix(screen_matrix);
        j = 0;
        count = (s32)(MODEL_PART_WORD(part) >> 53);
        block = *(ModelPartData **)&MODEL_PART_WORD(part);
        for (; j < count; j++) {
            s32 vertex_count = block->vertex_count & 0x7FFF;
            ModelPartVertexWords *vertex = (ModelPartVertexWords *)block->vertices;
            s32 setup = 2;
            ModelScreenPoint *screen;
            s32 outside;
            s32 k;

            length = block->length;
            screen = MODEL_CLIP_SCREEN_POINTS;

            for (k = vertex_count / 3; --k != -1;) { // match: this form of the countdown
                point0[0] = vertex->x;
                point0[1] = vertex->y;
                point0[2] = vertex->z;
                point0[3] = 1.0f;
                vertex++;
                point1[0] = vertex->x;
                point1[1] = vertex->y;
                point1[2] = vertex->z;
                point1[3] = 1.0f;
                vertex++;
                point2[0] = vertex->x;
                point2[1] = vertex->y;
                point2[2] = vertex->z;
                point2[3] = 1.0f;
                vertex++;
                Vu0_ProjectToScreen3((int *)screen++, (int *)screen++, (int *)screen++, point0, point1, point2);
            }
            for (k = vertex_count % 3; --k != -1;) {
                point0[0] = vertex->x;
                point0[1] = vertex->y;
                point0[2] = vertex->z;
                point0[3] = 1.0f;
                vertex++;
                Vu0_ProjectToScreen((int *)screen++, point0);
            }
            clip[0] = clip[1] = clip[2] = 0;
            outside = 0;
            vertex = (ModelPartVertexWords *)block->vertices;
            screen = MODEL_CLIP_SCREEN_POINTS;
            CLIP_SIDES(screen, clip[2]);
            if (OUT_OF_RANGE(screen)) {
                outside = 4;
            }
            vertex++;
            screen++;
            CLIP_SIDES(screen, clip[1]);
            if (OUT_OF_RANGE(screen)) {
                outside |= 2;
            }
            vertex++;
            screen++;
            for (k = 2; k < vertex_count; k++, vertex++, screen++) {
                CLIP_SIDES(screen, clip[0]);
                if (OUT_OF_RANGE(screen)) {
                    outside |= 1;
                }
                if (vertex->cut_flag == 0 && outside != 0 && outside != 7) {
                    if (!(clip[0] & clip[1] & clip[2])) {
                        if (clip[0] | clip[1] | clip[2]) {
                            ModelScreen_QueueCutTriangle((u32 *)block, vertex, first, setup, local_matrix);
                            first = FALSE;
                            if (setup != 0) {
                                setup--;
                            }
                        }
                    }
                }
                outside = (outside & 3) << 1;
                clip[2] = clip[1];
                clip[1] = clip[0];
                clip[0] = 0;
            }
            block = (ModelPartData *)((char *)block + (length + 1) * 16);
        }
    }
}

/* Queues one triangle of a block, the one that ends at `vertex`, cut up into 16 smaller ones:
   the edge between its last two vertices is cut in four, and so is each line from the first
   vertex (the apex) to a point of that edge; each pair of neighbouring lines goes out as a strip.
   `data` is the block (a ModelPartData) as words. `first` sends the matrix along, `setup` the
   block's GS setup. */
void ModelScreen_QueueCutTriangle(u32 *data, ModelPartVertexWords *vertex, int first, int setup, void *matrix) {
    u16 steps = 4;
    u16 vertex_count = steps * (steps * 2 + 1);
    int size;
    u32 *packet_start;
    ModelPartVertexWords *before = vertex - 1;
    ModelPartVertexWords *apex = vertex - 2;
    ModelPartVertexWords *edge_point;
    u32 *packet;
    ModelPartVertexWords *previous_point;
    ModelPartVertexWords *out;
    ModelPartVertexWords *row = MODEL_CUT_ROW_A;
    ModelPartVertexWords *previous = MODEL_CUT_ROW_B;
    ModelPartVertexWords *row_point;
    int i;
    int k;

    size = 1;
    if (setup) {
        size = 6;
    }
    size += vertex_count * 2 + 5;
    packet_start = packet = Packet_Alloc(first ? size + 6 : size);
    if (first) {
        *packet = 0x10000005;
        packet += 7;
        *packet++ = 0x6C040004;
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%0)\n"
            "lqc2 $vf17, 0x10(%0)\n"
            "lqc2 $vf18, 0x20(%0)\n"
            "lqc2 $vf19, 0x30(%0)\n"
            "sqc2 $vf16, 0x0(%1)\n"
            "sqc2 $vf17, 0x10(%1)\n"
            "sqc2 $vf18, 0x20(%1)\n"
            "sqc2 $vf19, 0x30(%1)\n"
            : : "r"(matrix), "r"(packet));
        packet += 0x10;
    }
    *packet = (size - 2) | DMA_TAG_ID_CNT;
    packet += 4;
    data += 4;
    if (setup) {
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
    } else {
        data += 20;
    }
    packet += 3;
    *packet++ = 0x6C01C000;
    data += 5;
    *packet++ = 0x8024;
    *packet++ = *data++;
    *packet++ = *data++;
    *packet = *data;
    packet += 4;
    *packet++ = (vertex_count << 18) | 0x6D008001;
    ModelScreen_StepVertices(MODEL_CUT_EDGE, before, vertex, steps);
    edge_point = MODEL_CUT_EDGE + 1;
    ModelScreen_StepVertices(previous, apex, MODEL_CUT_EDGE, steps);
    out = (ModelPartVertexWords *)packet;
    for (i = 1; i <= steps; i++) {
        row_point = row; // match: one variable swaps the rows and then walks the older one
        row = previous;
        previous = row_point;
        ModelScreen_StepVertices(row_point, apex, edge_point++, steps);
        *out = *apex;
        out->cut_flag = 0x8000;
        out++;
        *out = row[1];
        out->cut_flag = 0x8000;
        out++;
        row_point = row + 2;
        *out++ = previous[1];
        previous_point = previous + 2;
        for (k = 1; k < steps; k++) {
            *out++ = *row_point++;
            *out++ = *previous_point++;
        }
    }
    out = (ModelPartVertexWords *)((u32 *)out + 3); // match: stepped in place, then handed back to packet
    packet = (u32 *)out;
    if (first) {
        *packet++ = 0x15000000;
    } else {
        *packet++ = 0x17000000;
    }
    Packet_AppendToChain(packet_start, packet);
    cut_triangle_count++;
}

/* Writes steps + 1 vertices that lead from one vertex to another in equal steps; the position is
   stepped as floats and cut off, the rest as whole numbers. */
void ModelScreen_StepVertices(ModelPartVertexWords *out, ModelPartVertexWords *from, ModelPartVertexWords *last, int steps) {
    ModelPartVertexWords step;
    f32 position[4];
    f32 delta[4];
    ModelPartVertexWords *previous;
    int i;

    position[0] = from->x;
    position[1] = from->y;
    position[2] = from->z;
    position[3] = 1.0f;
    delta[0] = (last->x - position[0]) / steps;
    delta[1] = (last->y - position[1]) / steps;
    delta[2] = (last->z - position[2]) / steps;
    step.normal[0] = (last->normal[0] - from->normal[0]) / steps;
    step.normal[1] = (last->normal[1] - from->normal[1]) / steps;
    step.normal[2] = (last->normal[2] - from->normal[2]) / steps;
    step.tex_u = (last->tex_u - from->tex_u) / steps;
    step.tex_v = (last->tex_v - from->tex_v) / steps;
    step.colour[0] = (last->colour[0] - from->colour[0]) / steps;
    step.colour[1] = (last->colour[1] - from->colour[1]) / steps;
    step.colour[2] = (last->colour[2] - from->colour[2]) / steps;
    step.colour[3] = (last->colour[3] - from->colour[3]) / steps;
    previous = out;
    *out = *from;
    out->unit_weight = 0x1000;
    out->cut_flag = 0;
    out->blend_fraction = 0;
    out++;
    for (i = 1; i < steps; i++) {
        Vector_Add(position, position, delta);
        out->x = position[0];
        out->y = position[1];
        out->z = position[2];
        out->built_between = 1;
        out->normal[0] = previous->normal[0] + step.normal[0];
        out->normal[1] = previous->normal[1] + step.normal[1];
        out->normal[2] = previous->normal[2] + step.normal[2];
        out->cut_flag = 0;
        out->tex_u = previous->tex_u + step.tex_u;
        out->tex_v = previous->tex_v + step.tex_v;
        out->unit_weight = 0x1000;
        out->blend_fraction = 0x7FFF;
        out->colour[0] = previous->colour[0] + step.colour[0];
        out->colour[1] = previous->colour[1] + step.colour[1];
        out->colour[2] = previous->colour[2] + step.colour[2];
        out->colour[3] = previous->colour[3] + step.colour[3];
        previous++;
        out++;
    }
    *out = *last;
    out->unit_weight = 0x1000;
    out->cut_flag = 0;
    out->blend_fraction = 0;
}

/* Writes steps + 1 vertices that lead from one vertex to another in equal steps; the position is
   worked out afresh for each as floats and rounded, the rest is stepped as whole numbers. */
void ModelScreen_StepVerticesRounded(ModelPartVertexWords *out, ModelPartVertexWords *from, ModelPartVertexWords *last, int steps) {
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
    step.normal[0] = (last->normal[0] - from->normal[0]) / steps;
    step.normal[1] = (last->normal[1] - from->normal[1]) / steps;
    step.normal[2] = (last->normal[2] - from->normal[2]) / steps;
    step.tex_u = (last->tex_u - from->tex_u) / steps;
    step.tex_v = (last->tex_v - from->tex_v) / steps;
    step.colour[0] = (last->colour[0] - from->colour[0]) / steps;
    step.colour[1] = (last->colour[1] - from->colour[1]) / steps;
    step.colour[2] = (last->colour[2] - from->colour[2]) / steps;
    step.colour[3] = (last->colour[3] - from->colour[3]) / steps;
    previous = out;
    *out = *from;
    out->unit_weight = 0x1000;
    out->cut_flag = 0;
    out->blend_fraction = 0;
    out++;
    for (i = 1; i < steps; i++) {
        current[0] = position[0] + delta[0] * i / steps;
        current[1] = position[1] + delta[1] * i / steps;
        current[2] = position[2] + delta[2] * i / steps;
        out->x = ROUND_HALF(current[0]);
        out->y = ROUND_HALF(current[1]);
        out->z = ROUND_HALF(current[2]);
        out->built_between = 1;
        out->normal[0] = previous->normal[0] + step.normal[0];
        out->normal[1] = previous->normal[1] + step.normal[1];
        out->normal[2] = previous->normal[2] + step.normal[2];
        out->cut_flag = 0;
        out->tex_u = previous->tex_u + step.tex_u;
        out->tex_v = previous->tex_v + step.tex_v;
        out->unit_weight = 0x1000;
        out->blend_fraction = 0x7FFF;
        out->colour[0] = previous->colour[0] + step.colour[0];
        out->colour[1] = previous->colour[1] + step.colour[1];
        out->colour[2] = previous->colour[2] + step.colour[2];
        out->colour[3] = previous->colour[3] + step.colour[3];
        previous++;
        out++;
    }
    *out = *last;
    out->unit_weight = 0x1000;
    out->cut_flag = 0;
    out->blend_fraction = 0;
}

/* Queues one triangle of a block, the one that ends at `vertex`, cut up into 16 smaller ones and
   with its vertices as floats (ModelScreen_QueueCutTriangle does the same with packed vertices): the edge between
   its last two vertices is cut in four, and so is each line from the first vertex (the apex) to a
   point of that edge; each pair of neighbouring lines goes out as a strip. `data` is the block (a
   ModelPartData) as words. `first` sends the matrix along, `setup` the block's GS setup. */
void ModelScreen_QueueCutFloatTriangle(u32 *data, ModelPartVertexWords *vertex, int first, int setup, void *matrix) {
    BlendVertex apex_point;
    BlendVertex before_point;
    BlendVertex vertex_point;
    u16 steps = 4;
    u16 vertex_count = steps * (steps * 2 + 1);
    int size;
    u32 *packet_start;
    ModelPartVertexWords *before = vertex - 1;
    ModelPartVertexWords *apex = vertex - 2;
    BlendVertex *edge_point;
    u32 *packet;
    BlendVertex *previous_point;
    BlendVertex *out;
    BlendVertex *row = MODEL_CUT_BLEND_ROW_A;
    BlendVertex *previous = MODEL_CUT_BLEND_ROW_B;
    BlendVertex *row_point;
    int i;
    int k;

    size = 1;
    if (setup) {
        size = 8;
    }
    size += vertex_count * 4 + 5;
    TO_BLEND_VERTEX(apex_point, apex);
    TO_BLEND_VERTEX(before_point, before);
    TO_BLEND_VERTEX(vertex_point, vertex);
    packet_start = packet = Packet_Alloc(first ? size + 10 : size);
    if (first) {
        *packet = 0x10000005;
        packet += 7;
        *packet++ = 0x6C040004;
        __asm__ volatile(
            "lqc2 $vf16, 0x0(%0)\n"
            "lqc2 $vf17, 0x10(%0)\n"
            "lqc2 $vf18, 0x20(%0)\n"
            "lqc2 $vf19, 0x30(%0)\n"
            "sqc2 $vf16, 0x0(%1)\n"
            "sqc2 $vf17, 0x10(%1)\n"
            "sqc2 $vf18, 0x20(%1)\n"
            "sqc2 $vf19, 0x30(%1)\n"
            : : "r"(matrix), "r"(packet));
        packet += 0x10;
        *packet = 0x10000003;
        packet += 4;
        *packet++ = 0x6C020064;
        *(f32 *)packet++ = 1.0f;
        *(f32 *)packet++ = 1.0f;
        *(f32 *)packet++ = 1.0f;
        *(f32 *)packet++ = 1.0f;
        *(f32 *)packet++ = 0.0f;
        *(f32 *)packet++ = 128.0f;
        *(f32 *)packet++ = 0.0f;
        *(f32 *)packet++ = 128.0f;
    }
    *packet = (size - 2) | DMA_TAG_ID_CNT;
    packet += 4;
    data += 4;
    if (setup) {
        *(u_long128 *)packet = 0x6C06C0A0;
        packet += 4;
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        COPY_QWORD(packet, data);
        *(u_long128 *)packet = 0;
        packet += 4;
        *(u_long128 *)packet = 0;
        packet += 4;
    } else {
        data += 20;
    }
    packet += 3;
    *packet++ = 0x6C01C000;
    data += 5;
    *packet++ = 0x8024;
    *packet++ = *data++;
    *packet++ = *data++;
    *packet = *data;
    packet += 4;
    *packet++ = (vertex_count << 18) | 0x6C008001;
    ModelScreen_StepFloatVertices(MODEL_CUT_BLEND_EDGE, &before_point, &vertex_point, steps);
    edge_point = MODEL_CUT_BLEND_EDGE + 1;
    ModelScreen_StepFloatVertices(MODEL_CUT_BLEND_ROW_B, &apex_point, MODEL_CUT_BLEND_EDGE, steps);
    out = (BlendVertex *)packet;
    for (i = 1; i <= steps; i++) {
        row_point = row; // match: one variable swaps the rows and then walks the older one
        row = previous;
        previous = row_point;
        ModelScreen_StepFloatVertices(row_point, &apex_point, edge_point++, steps);
        *out = apex_point;
        *(s32 *)&out->normal[3] = 0x8000;
        out++;
        *out = row[1];
        *(s32 *)&out->normal[3] = 0x8000;
        out++;
        row_point = row + 2;
        *out++ = previous[1];
        previous_point = previous + 2;
        for (k = 1; k < steps; k++) {
            *out++ = *row_point++;
            *out++ = *previous_point++;
        }
    }
    out = (BlendVertex *)((u32 *)out + 3); // match: stepped in place, then handed back to packet
    packet = (u32 *)out;
    if (first) {
        *packet++ = 0x15000280;
    } else {
        *packet++ = 0x17000000;
    }
    Packet_AppendToChain(packet_start, packet);
    cut_triangle_count++;
}

/* Writes steps + 1 float vertices that lead from one packed vertex to another in equal steps
   (ModelScreen_StepFloatVertices does the same between two float vertices). */
void ModelScreen_StepPackedToFloat(BlendVertex *out, ModelPartVertexWords *from, ModelPartVertexWords *last, int steps) {
    BlendVertex from_point;
    BlendVertex last_point;
    BlendVertex delta;
    BlendVertex *previous;
    f32 scale;
    int i;

    scale = 1.0f / steps;
    TO_BLEND_VERTEX(from_point, from);
    TO_BLEND_VERTEX(last_point, last);
    Vector_Subtract(delta.position, last_point.position, from_point.position);
    Vector_Scale(delta.position, delta.position, scale);
    Vector_Subtract(delta.normal, last_point.normal, from_point.normal);
    Vector_Scale(delta.normal, delta.normal, scale);
    delta.tex_coords[0] = last_point.tex_coords[0] * from_point.tex_coords[0] * scale;
    delta.tex_coords[1] = last_point.tex_coords[1] * from_point.tex_coords[1] * scale;
    Vector_Subtract(delta.colour, last_point.colour, from_point.colour);
    Vector_Scale(delta.colour, delta.colour, scale);
    previous = out;
    *out = from_point;
    out++;
    for (i = 1; i < steps; i++) {
        Vector_Add(out->position, previous->position, delta.position);
        out->position[3] = 1.0f;
        Vector_Add(out->normal, previous->normal, delta.normal);
        *(s32 *)&out->normal[3] = 0; // match: stored as an integer; as a float it would hold the loads below back
        out->tex_coords[0] = previous->tex_coords[0] + delta.tex_coords[0];
        out->tex_coords[1] = previous->tex_coords[1] + delta.tex_coords[1];
        out->tex_coords[2] = 1.0f;
        *(s32 *)&out->tex_coords[3] = 0; // match: an integer as well
        Vector_Add(out->colour, previous->colour, delta.colour);
        previous++;
        out++;
    }
    *out = last_point;
}

/* Writes steps + 1 vertices that lead from one vertex to another in equal steps
   (ModelScreen_StepPackedToFloat does the same between two packed vertices). */
void ModelScreen_StepFloatVertices(BlendVertex *out, BlendVertex *from, BlendVertex *last, int steps) {
    BlendVertex delta;
    BlendVertex *previous;
    f32 scale;
    int i;

    scale = 1.0f / steps;
    Vector_Subtract(delta.position, last->position, from->position);
    Vector_Scale(delta.position, delta.position, scale);
    Vector_Subtract(delta.normal, last->normal, from->normal);
    Vector_Scale(delta.normal, delta.normal, scale);
    // match: retail bug, a product where every other component uses the difference last - from
    delta.tex_coords[0] = last->tex_coords[0] * from->tex_coords[0] * scale;
    delta.tex_coords[1] = last->tex_coords[1] * from->tex_coords[1] * scale; // match: retail bug, as above
    Vector_Subtract(delta.colour, last->colour, from->colour);
    Vector_Scale(delta.colour, delta.colour, scale);
    previous = out;
    *out = *from;
    out++;
    for (i = 1; i < steps; i++) {
        Vector_Add(out->position, previous->position, delta.position);
        out->position[3] = 1.0f;
        Vector_Add(out->normal, previous->normal, delta.normal);
        *(s32 *)&out->normal[3] = 0; // match: stored as an integer; as a float it would hold the loads below back
        out->tex_coords[0] = previous->tex_coords[0] + delta.tex_coords[0];
        out->tex_coords[1] = previous->tex_coords[1] + delta.tex_coords[1];
        out->tex_coords[2] = 1.0f;
        *(s32 *)&out->tex_coords[3] = 0; // match: an integer as well
        Vector_Add(out->colour, previous->colour, delta.colour);
        previous++;
        out++;
    }
    *out = *last;
}

/* Gives every vertex texture coordinates taken from where it lands on the screen. Also steps a
   60-frame counter and fills a sine table that nothing reads. */
void ModelScreen_SetUvsFromScreen(ModelInstance *model) {
    VuVector4 matrix[4];
    s32 screen[4];
    f32 point[4];
    f32 sines[60];
    ModelPart *part;
    ModelPartData *next_data;
    ModelPartData *data;
    ModelPartVertex *vertex;
    int i;
    int j;
    int k;
    int count;
    int part_count;
    int vertex_count;
    u16 length;

    screen_uv_frame_copy = screen_uv_frame = (screen_uv_frame + 1) % 60;
    for (i = 0; i < 60; i++) {
        sines[i] = sin((f32)i * 6.2831855f / 60.0f) * 10.0;
    }
    part = model->parts;
    part_count = model->part_count;
    for (i = 0; i < part_count; i++, part++) {
        FlushCache(0);
        sceVu0MulMatrix(matrix, world_screen_matrix, part);
        Vu0_LoadMatrix(matrix);
        ((void (*)(int))FlushCache)(0); // match: called as void, retail reuses $2 at once
        count = part->data_count;
        next_data = (ModelPartData *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            length = next_data->length;
            data = next_data;
            next_data = (ModelPartData *)&next_data->vertex_count; // match: the cursor steps on to the vertex count
            vertex_count = ((ModelPartDataTail *)next_data)->vertex_count & 0x7FFF;
            vertex = ((ModelPartDataTail *)next_data)->vertices;
            for (k = 0; k < vertex_count; k++) {
                point[0] = vertex->x;
                point[1] = vertex->y;
                point[2] = vertex->z;
                point[3] = 1.0f;
                Vu0_ProjectToScreen(screen, point);
                vertex->tex_u = (s16)(screen[0] - 0x6C00) * 6.1035156e-05f;
                vertex->tex_v = (s16)(screen[1] - 0x7200) * 0.00012207031f;
                vertex->uvs_set_from_screen = 1;
                vertex++;
            }
            next_data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
}
