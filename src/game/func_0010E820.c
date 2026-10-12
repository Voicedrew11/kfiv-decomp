#include "kf_model.h"

/* Set the doubleword at +0x50 of every data block of every part. */
void func_0010E820(ModelInstance *model, s64 value) {
    s32 i = 0;
    u16 part_count = model->part_count;
    ModelPart *part = model->parts;

    for (; i < part_count; i++, part++) {
        s32 j = 0;
        s32 count = (s32)(MODEL_PART_WORD(part) >> 53);
        u64 *cursor = *(u64 **)&MODEL_PART_WORD(part);

        for (; j < count; j++) {
            ModelPartData *block = (ModelPartData *)cursor;
            u16 length = block->length;

            cursor = &block->unk50;
            *cursor = value;
            cursor = (u64 *)((char *)block + (length + 1) * 16);
        }
    }
}
