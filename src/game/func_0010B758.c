#include "kf_model.h"

void func_0010BA30(ModelInstance *);
void func_0010C9E8(ModelInstance *);
// match: gcse numbers the registers of the three function addresses below by a hash of the symbol names (and of
// the function's size), and the original names are not known. The '*' an asm label puts in front of this one
// name gives retail's order ($24, $16, $25). Revisit when any of the three functions is renamed.
char *func_0010CAB8(char *, u32) __asm__("func_0010CAB8");

/* Set a model instance up from a model file: its parts are carved out of *memory, and both
   cursors move past what was used. */
void func_0010B758(ModelInstance *model, int *file_cursor, int *memory) {
    ModelFile *file;
    ModelFilePart *file_part;
    ModelPart *memory_base;
    char *cursor;
    ModelPart *part;
    Vector4 *positions;
    ModelPartData *data;
    s32 part_count;
    u16 length;
    s32 first;
    s32 i;
    s32 j;

    model->unk100 |= 0x4000;
    cursor = (char *)*file_cursor;
    memory_base = (ModelPart *)*memory;
    model->file = (ModelFile *)cursor;
    file = (ModelFile *)cursor;
    part_count = file->part_count;
    cursor = (char *)file->bone_anchors;
    model->bone_anchors = (BoneAnchor *)cursor;
    model->part_count = model->unkF2 = file->part_count;
    file_part = file->parts;
    positions = (Vector4 *)&memory_base[part_count];
    model->parts = memory_base;
    model->bone_positions = positions;
    *memory = (int)(positions + 32);
    part = memory_base;
    for (i = 0; i < file->part_count; i++, part++, file_part++) {
        part->data_count = file_part->data_count;
        part->data_addr = *file_cursor + file_part->data_offset;
        part->pad78_46 = file_part->unk07;
        MODEL_PART_FLAG_BITS(part)->bit41 = TRUE;
        // match: retail keeps only the empty loop; a walk whose result nothing uses is deleted like this one
        data = (ModelPartData *)(u32)part->data_addr;
        for (j = 0; j < file_part->data_count; j++) {
            length = data->length;
            data = (ModelPartData *)((char *)data + length * 16);
        }
        part->unk40[0] = part->unk40[1] = part->unk40[2] = 0.0f;
        part->unk40[3] = 1.0f;
        part->unk50 = part->unk54 = part->unk58 = 0.0f;
        part->unk5C = 1.0f;
        part->unk60[0] = part->unk60[1] = part->unk60[2] = 1.0f;
        if (file_part->unk06 >= 1 && file_part->unk06 <= 3) {
            MODEL_SLOT_PARTS(model)[file_part->unk06 - 1] = part;
        }
    }
    model->unk1A8 = func_0010BA30;
    *file_cursor += file->size;
    model->unk1B4 = func_0010C9E8;
    model->get_point = (int (*)(void *, int))func_0010CAB8;
    first = TRUE;
    model->unk160[0] = 0;
    model->unk160[1] = 0;
    model->unk160[2] = 0;
    model->unk160[3] = 0;
    model->unk150 = 128.0f;
    model->unk154 = 128.0f;
    model->unk158 = 128.0f;
    model->alpha = 128.0f;
    /* the first block with a nonzero unk37 in the whole model gets unk61 = 2 */
    for (i = 0, part = model->parts; i < file->part_count; i++, part++) {
        data = (ModelPartData *)(u32)part->data_addr;
        for (j = 0; j < part->data_count; j++) {
            length = data->length;
            if (first && data->unk30.unk37 != 0) {
                first = FALSE;
                data->unk30.unk61 = 2;
            }
            data = (ModelPartData *)((char *)data + length * 16);
        }
    }
}
