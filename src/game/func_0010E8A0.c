#include "kf_model.h"

/* Set the word that ends the first data block of every part. */
void func_0010E8A0(ModelInstance *model, s32 value) {
    ModelPart *part;
    ModelPartData *data;
    u32 *last_qword;
    s32 i;

    part = model->parts;
    for (i = 0; i < model->part_count; i++, part++) {
        data = (ModelPartData *)(u32)part->data_addr;
        last_qword = (u32 *)((char *)data + data->length * 16);
        last_qword[3] = value | 0x15000000;
    }
}
