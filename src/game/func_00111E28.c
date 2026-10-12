#include "kf_model.h"

int rand(void);
void srand(u32);

/* the address rounded up to a multiple of 16 */
static inline u32 align_16(u32 address) {
    u32 remainder = address & 0xF;
    u32 aligned = address;

    if (remainder != 0) {
        aligned = remainder - 0x10;
        aligned = address - aligned;
    }
    return aligned;
}

/* Builds the model's table of wave texture coordinates at *alloc (16-byte aligned) and moves *alloc
   past it. Each vertex keeps its own coordinates and gets a wave picked from where it is. */
void func_00111E28(ModelInstance *model, u32 *alloc) {
    ModelPart *part;
    ModelPartData *next_data;
    ModelPartData *data;
    ModelPartVertex *vertex;
    ModelWaveUv *wave_uv;
    int i;
    int j;
    int k;
    int count;
    int part_count;
    int vertex_count;
    u16 length;
    u32 address;
    u32 remainder;

    address = align_16(*alloc);
    model->wave_uvs = (ModelWaveUv *)address;
    part = model->parts;
    part_count = model->part_count;
    wave_uv = (ModelWaveUv *)address;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        next_data = (ModelPartData *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            data = next_data;
            next_data = (ModelPartData *)next_data->vertices; // match: the cursor steps on to the vertices
            vertex = (ModelPartVertex *)next_data;
            vertex_count = data->unk70 & 0x7FFF;
            length = data->length;
            for (k = 0; k < vertex_count; k++) {
                wave_uv->u = vertex->u;
                wave_uv->v = vertex->v;
                srand((vertex->x + (int)model->position.x) + ((int)model->position.y + vertex->y) +
                      ((int)model->position.z + vertex->z));
                wave_uv->wave = rand() % 16;
                wave_uv++;
                vertex++;
            }
            next_data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
    address = (u32)wave_uv;
    remainder = address & 0xF;
    if (remainder != 0) {
        remainder -= 0x10;
        address -= remainder;
    }
    *alloc = address;
}
