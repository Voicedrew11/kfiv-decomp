#include "kf_gs.h"
#include "kf_model.h"

/* whether the primitive of a GIF tag is alpha blended, from the tag's high word */
static inline s32 is_blended(u64 tag_high) {
    return (tag_high >> 21) & 1;
}

void func_0010D308(ModelInstance *);
void func_0010E5A0(char *);
Vector4 *func_0010E678(ModelInstance *, s32);

/* Set a model instance up from a model file whose parts come with their pose: the parts are
   carved out of *memory, and both cursors move past what was used. */
void func_0010D018(ModelInstance *model, char **file_cursor, char **memory) {
    PosedModelFile *file;
    PosedModelFilePart *file_part;
    ModelPart *part;
    ModelPartData *data;
    s32 first = TRUE;
    s32 i = 0;
    s32 j;
    s32 opaque;
    s32 blended;
    s32 count;
    u16 length;
    char *base;
    char *memory_base;
    char *addr;

    base = *file_cursor;
    memory_base = *memory;
    model->file = (ModelFile *)base;
    file = (PosedModelFile *)base;
    model->parts = (ModelPart *)memory_base;
    base = (char *)((model->unkF2 = file->part_count) * sizeof(ModelPart)); // match: base reused for the size, the count stored on the way
    *memory = memory_base + (u32)base;
    model->part_count = model->unkF2; // match: copied from the store above, not from the file
    part = (ModelPart *)memory_base;
    file_part = file->parts;
    if (POSED_MODEL_FILE_ANY(file).part_count != 0) { // match: the test reads the count after the cursor store
        do {
            part->data_count = file_part->data_count;
            memory_base = *file_cursor + file_part->data_offset;
            part->data_addr = (u32)memory_base;
            blended = is_blended(((ModelPartData *)memory_base)->unk74);
            data = (ModelPartData *)memory_base;
            opaque = FALSE;
            if (!blended || ((GsAlphaReg *)&data->unk50)->f.a_source != 0 || ((GsAlphaReg *)&data->unk50)->f.b_source != 2 || ((GsAlphaReg *)&data->unk50)->f.d_source != 1) {
                opaque = TRUE;
            }
            MODEL_PART_FLAG_BITS(part)->bit41 = opaque;
            if (first) {
                count = file_part->data_count;
                data = (ModelPartData *)(u32)part->data_addr;
                for (j = 0; j < count; j++) {
                    length = data->length;
                    if (((ModelPartDataBits30 *)&data->unk40)->unk37 != 0) {
                        ((ModelPartDataBits30 *)&data->unk40)->unk61 = 2;
                        first = FALSE;
                        break;
                    }
                    data = (ModelPartData *)((char *)data + (length + 1) * 16);
                }
            }
            part->unk40[0] = file_part->position[0];
            part->unk40[1] = file_part->position[1];
            part->unk40[2] = file_part->position[2];
            part->unk40[3] = 1.0f;
            part->unk50 = file_part->rotation[0];
            part->unk54 = file_part->rotation[1];
            part->unk58 = file_part->rotation[2];
            part->unk5C = 1.0f;
            part->unk60[0] = file_part->scale[0];
            part->unk60[1] = file_part->scale[1];
            part->unk60[2] = file_part->scale[2];
            part++;
            file_part++;
            i++;
        } while (i < file->part_count);
    }
    if (file->bone_anchors_offset != 0) {
        addr = *file_cursor;
        model->bone_positions = (Vector4 *)*memory;
        *memory += 32 * sizeof(Vector4);
        model->bone_anchors = (BoneAnchor *)(addr + file->bone_anchors_offset);
    } else {
        model->bone_anchors = NULL;
    }
    model->unk1A8 = func_0010D308;
    *file_cursor += file->size;
    model->unk1B4 = (void (*)(ModelInstance *))func_0010E5A0;
    model->get_point = (int (*)(void *, int))func_0010E678;
}
