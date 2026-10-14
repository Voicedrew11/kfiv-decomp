#include "kf_model.h"

extern f32 D_004323E0[16];

int rand(void);
void Matrix_Multiply(void *, void *, void *);
void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);

/* Gives every vertex texture coordinates taken from where it lands on the screen, with each
   vertex shaken a little first. */
void func_00112118(ModelInstance *model) {
    ModelPart *part;
    ModelPartData *next_data;
    ModelPartData *data;
    ModelPartVertex *vertex;
    f32 *matrix;
    int i;
    int j;
    int k;
    int count;
    int part_count;
    int vertex_count;
    u16 length;
    ModelProjectScratch *scratch = MODEL_PROJECT_SCRATCH;

    part = model->parts;
    part_count = model->part_count;
    matrix = MODEL_PROJECT_SCRATCH->matrices[0];
    for (i = 0; i < part_count; i++, part++) {
        Matrix_Multiply(matrix, model->unk40, part);
        Matrix_Multiply(matrix, D_004323E0, matrix);
        matrix += 16;
    }
    part = model->parts;
    part_count = model->part_count;
    for (i = 0; i < part_count; i++, part++) {
        Vu0_LoadMatrix(scratch->matrices[i]);
        count = part->data_count;
        next_data = (ModelPartData *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            data = next_data;
            next_data = (ModelPartData *)next_data->vertices; // match: the cursor steps on to the vertices
            vertex = (ModelPartVertex *)next_data;
            vertex_count = data->unk70 & 0x7FFF;
            length = data->length;
            for (k = 0; k < vertex_count; k++) {
                *&MODEL_PROJECT_SCRATCH->point[0] = vertex->x + (rand() - 1073741824.0f) * 2.0f * 9.313226e-10f;
                *&MODEL_PROJECT_SCRATCH->point[1] = vertex->y + (rand() - 1073741824.0f) * 2.0f * 9.313226e-10f;
                *&MODEL_PROJECT_SCRATCH->point[2] = vertex->z + (rand() - 1073741824.0f) * 2.0f * 9.313226e-10f;
                *&MODEL_PROJECT_SCRATCH->point[3] = 1.0f;
                Vu0_ProjectToScreen(MODEL_PROJECT_SCRATCH->screen, MODEL_PROJECT_SCRATCH->point);
                vertex->tex_u = (*&MODEL_PROJECT_SCRATCH->screen[0] - 0x6C00) / 10240.0f * 4096.0f;
                vertex->tex_v = (*&MODEL_PROJECT_SCRATCH->screen[1] - 0x7200) / 7168.0f * 4096.0f;
                vertex++;
            }
            next_data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
}
