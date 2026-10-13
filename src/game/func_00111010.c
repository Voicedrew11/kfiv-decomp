#include "kf_model.h"

extern f32 D_004323E0[16];
extern int D_00446760;
extern int D_00447054;

double sin(double);
int FlushCache(int);
void sceVu0MulMatrix(void *, void *, void *);
void Vu0_LoadMatrix(void *);
void Vu0_ProjectToScreen(int *, void *);

/* Gives every vertex texture coordinates taken from where it lands on the screen. Also steps a
   60-frame counter and fills a sine table that nothing reads. */
void func_00111010(ModelInstance *model) {
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

    D_00447054 = D_00446760 = (D_00446760 + 1) % 60;
    for (i = 0; i < 60; i++) {
        sines[i] = sin((f32)i * 6.2831855f / 60.0f) * 10.0;
    }
    part = model->parts;
    part_count = model->part_count;
    for (i = 0; i < part_count; i++, part++) {
        FlushCache(0);
        sceVu0MulMatrix(matrix, D_004323E0, part);
        Vu0_LoadMatrix(matrix);
        ((void (*)(int))FlushCache)(0); // match: called as void, retail reuses $2 at once
        count = part->data_count;
        next_data = (ModelPartData *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            length = next_data->length;
            data = next_data;
            next_data = (ModelPartData *)&next_data->unk70; // match: the cursor steps on to the vertex count
            vertex_count = ((ModelPartDataTail *)next_data)->unk00 & 0x7FFF;
            vertex = ((ModelPartDataTail *)next_data)->vertices;
            for (k = 0; k < vertex_count; k++) {
                point[0] = vertex->x;
                point[1] = vertex->y;
                point[2] = vertex->z;
                point[3] = 1.0f;
                Vu0_ProjectToScreen(screen, point);
                vertex->tex_u = (s16)(screen[0] - 0x6C00) * 6.1035156e-05f;
                vertex->tex_v = (s16)(screen[1] - 0x7200) * 0.00012207031f;
                vertex->unk14 = 1;
                vertex++;
            }
            next_data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
}
