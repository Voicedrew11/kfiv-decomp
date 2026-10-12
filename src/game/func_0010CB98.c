#include "kf_model.h"

extern f32 D_004323E0[16];

void func_00216B48(void *, void *, void *);
void func_00216C48(void *, void *, void *);
void KF_Vu0_LoadMatrix(void *);
void KF_Vu0_ProjectToScreen(int *, void *);

/* Gives every vertex of a model drawn with its own texture the texture coordinates of the place
   it lands on the screen (0 to 1 across the screen). */
void func_0010CB98(void *model_arg) {
    ModelInstance *model = model_arg;
    ModelPart *part;
    f32 *matrix;
    int count;
    f32 *parent_matrix;
    int part_count;
    int j;
    int parent;
    int i; // match: declared after j and parent, which decides the order of the stack slots
    ModelProjectScratch *scratch = MODEL_PROJECT_SCRATCH;

    part = model->parts;
    part_count = model->part_count;
    matrix = MODEL_PROJECT_SCRATCH->matrices[0];
    for (i = 0; i < part_count; i++, part++) {
        func_00216B48((int)matrix, (int *)model->unk40, (int)part);
        func_00216B48((int)matrix, (int *)D_004323E0, (int)matrix);
        matrix += 16;
    }
    i = 0;
    part_count = model->part_count;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        s32 last;
        s32 *cursor;

        matrix = scratch->matrices[i];
        parent = (s32)(MODEL_PART_WORD(part) >> 46) & 0x7F; /* ModelPart.pad78_46: the parent part */
        if (parent != -1) { // match: always true, the 7-bit field holds 0x7F for "no parent" (see func_0010B758)
            parent_matrix = scratch->matrices[parent];
        }
        count = (s32)(MODEL_PART_WORD(part) >> 53);
        last = -1;
        j = 0;
        cursor = *(s32 **)&MODEL_PART_WORD(part);
        for (; j < count; j++) {
            ModelWideData *block = (ModelWideData *)cursor;
            u16 length = block->length;
            s32 k = 0;
            s32 vertex_count;
            ModelWideVertex *vertex;

            cursor = &block->unk90;
            vertex_count = *cursor & 0x7FFF;
            vertex = block->vertices;
            for (; k < vertex_count; k++, vertex++) {
                if (last != vertex->on_parent) {
                    if (vertex->on_parent != 0) {
                        KF_Vu0_LoadMatrix(parent_matrix);
                    } else {
                        KF_Vu0_LoadMatrix(matrix);
                    }
                    last = vertex->on_parent;
                }
                func_00216C48(MODEL_PROJECT_SCRATCH->point, vertex->position, vertex->offset);
                KF_Vu0_ProjectToScreen(MODEL_PROJECT_SCRATCH->screen, MODEL_PROJECT_SCRATCH->point);
                vertex->tex_s = (*&MODEL_PROJECT_SCRATCH->screen[0] - 0x6C00) / 10240.0f;
                vertex->tex_t = (*&MODEL_PROJECT_SCRATCH->screen[1] - 0x7200) / 7168.0f;
            }
            cursor = (s32 *)((char *)block + (length + 1) * 16);
        }
    }
}
