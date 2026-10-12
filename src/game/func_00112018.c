#include "kf_screen.h"

extern WaveOscillator D_00471DE0[];

/* Moves every vertex's texture coordinates by its wave. */
void func_00112018(ModelInstance *model) {
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

    part = model->parts;
    part_count = model->part_count;
    wave_uv = model->wave_uvs;
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
                vertex->u = wave_uv->u + (u16)D_00471DE0[wave_uv->wave].x;
                vertex->v = wave_uv->v + (u16)D_00471DE0[wave_uv->wave].y;
                wave_uv++;
                vertex++;
            }
            next_data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
}
