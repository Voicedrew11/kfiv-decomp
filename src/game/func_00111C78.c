#include "kf_model.h"

/* Whether any data block of the model draws with a texture numbered from first to last. */
int func_00111C78(ModelInstance *model, int first, int last) {
    ModelPart *part;
    ModelPartData *data;
    int i;
    int j;
    int count;
    int part_count;
    int texture;
    u16 length;

    part_count = model->part_count;
    part = model->parts;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        data = (ModelPartData *)(int)part->data_addr;
        for (j = 0; j < count; j++) {
            texture = data->unk40 & 0x3FFF;
            length = data->length;
            if (texture >= first && texture <= last) {
                return TRUE;
            }
            data = (ModelPartData *)((char *)data + (length + 1) * 0x10);
        }
    }
}
