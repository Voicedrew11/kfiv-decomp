#include "kf_model.h"

/* Set the colour of every vertex of a model; a component outside 0..0xFF is left as it is. */
void func_0010E8E0(void *model_arg, int red, int green, int blue, int alpha) {
    ModelInstance *model = model_arg;
    s16 scaled[4];
    s32 i = 0;
    u16 part_count = model->part_count;
    ModelPart *part;

    scaled[0] = red << 12;
    scaled[1] = green << 12;
    scaled[2] = blue << 12;
    scaled[3] = alpha << 12;
    part = model->parts;
    for (; i < part_count; i++, part++) {
        s32 j = 0;
        s32 count = (s32)(MODEL_PART_WORD(part) >> 53);
        s32 *cursor = *(s32 **)&MODEL_PART_WORD(part);

        for (; j < count; j++) {
            ModelPartData *block = (ModelPartData *)cursor;
            u16 length = block->length;
            s32 k = 0;
            s32 vertex_count;
            ModelPartVertex *vertex;

            cursor = &block->unk70;
            vertex_count = *cursor & 0x7FFF;
            vertex = block->vertices;
            for (; k < vertex_count; k++, vertex++) {
                if ((u32)red < 0x100) {
                    vertex->colour[0] = red;
                }
                if ((u32)green < 0x100) {
                    vertex->colour[1] = green;
                }
                if ((u32)blue < 0x100) {
                    vertex->colour[2] = blue;
                }
                if ((u32)alpha < 0x100) {
                    vertex->colour[3] = alpha;
                }
            }
            cursor = (s32 *)((char *)block + (length + 1) * 16);
        }
    }
}
