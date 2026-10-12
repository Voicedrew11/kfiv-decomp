#include "kf_model.h"

/* Set the doublewords at +0x30 and +0x40 of every data block of every part. When bits 37-50 of
   the second are not 0, the model's first block gets it with 2 in its top three bits. */
void func_0010E770(ModelInstance *model, u64 unk40, s64 unk30) {
    ModelPart *part;
    ModelPartData *data;
    ModelPartData *block;
    s32 part_count;
    s32 count;
    s32 first;
    s32 i;
    s32 j;
    u16 length;

    first = (s32)((unk40 >> 37) & 0x3FFF) != 0;
    part = model->parts;
    part_count = model->part_count;
    for (i = 0; i < part_count; i++, part++) {
        count = part->data_count;
        data = (ModelPartData *)(u32)part->data_addr;
        for (j = 0; j < count; j++) {
            block = data;
            length = block->length;
            *(s64 *)&block->unk30 = unk30;
            block->unk40 = unk40;
            if (first) {
                ((ModelPartDataBits30 *)&block->unk40)->unk61 = 2;
                first = FALSE;
            }
            data = (ModelPartData *)((char *)block + (length + 1) * 16);
        }
    }
}
