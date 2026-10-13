#include "kf_model.h"

extern f32 D_004323E0[16];      /* world to screen */

void sceVu0MulMatrix(void *, void *, void *);
void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);
void func_002168B8(int *, int *, int *, f32 *, f32 *, f32 *);
void func_0010F478(u32 *, ModelPartVertexWords *, int, int, void *);

/* the scratchpad as this function uses it: where each vertex of a block lands on the screen */
#define SCREEN_POINTS ((ModelScreenPoint *)0x70000000)

/* which sides of the screen a point is beyond */
#define CLIP_SIDES(point, sides)                                                                   \
    if ((point)->x < 0x6C00) {                                                                   \
        (sides) |= 1;                                                                              \
    }                                                                                              \
    if ((point)->x > 0x9400) {                                                                   \
        (sides) |= 2;                                                                              \
    }                                                                                              \
    if ((point)->y < 0x7200) {                                                                   \
        (sides) |= 4;                                                                              \
    }                                                                                              \
    if ((point)->y > 0x8E00) {                                                                   \
        (sides) |= 8;                                                                              \
    }

/* whether a point is too deep or outside the range the GS can draw */
#define OUT_OF_RANGE(point) ((point)->z > 0xFFFF0 || (((point)->x | (point)->y) & 0xFFFF0000))

/* Looks for the triangles of a model that the GS could not draw as they are (a corner out of its
   range) but that are partly on the screen, and queues each cut up into smaller ones. */
void func_0010EE98(ModelInstance *model) {
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

        sceVu0MulMatrix(local_matrix, model->unk40, part);
        sceVu0MulMatrix(screen_matrix, D_004323E0, local_matrix);
        Vu0_LoadMatrix(screen_matrix);
        j = 0;
        count = (s32)(MODEL_PART_WORD(part) >> 53);
        block = *(ModelPartData **)&MODEL_PART_WORD(part);
        for (; j < count; j++) {
            s32 vertex_count = block->unk70 & 0x7FFF;
            ModelPartVertexWords *vertex = (ModelPartVertexWords *)block->vertices;
            s32 setup = 2;
            ModelScreenPoint *screen;
            s32 outside;
            s32 k;

            length = block->length;
            screen = SCREEN_POINTS;

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
                func_002168B8((int *)screen++, (int *)screen++, (int *)screen++, point0, point1, point2);
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
            screen = SCREEN_POINTS;
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
                if (vertex->unk0E == 0 && outside != 0 && outside != 7) {
                    if (!(clip[0] & clip[1] & clip[2])) {
                        if (clip[0] | clip[1] | clip[2]) {
                            func_0010F478((u32 *)block, vertex, first, setup, local_matrix);
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
