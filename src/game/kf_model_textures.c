#include "common.h"
#include "kf_model.h"
#include "kf_texture.h"

void *memset(void *, int, u32);
void GsDraw_QueueTexFlush(void);
void TextureTable_QueueEnabled(TextureTableHeader *, s32);
void Map_QueuePacket(int *);

static int sent_texture_19C;        /* 0x447048 last model texture sent from unk19C */
static int sent_texture_1A0;        /* 0x44704C last model texture sent from unk1A0 */
/* 0x447050 the model texture taken as sent from unk198, by unk192; only [0] is read, and nothing but the reset
   writes it. A 4-byte object: [1] would be screen_uv_frame_copy (kf_model_screen.c), so it stays extern. */
extern int D_00447050[2];
static int texture_slots[4];        /* 0x468190 chunk table last sent for each unk190 slot */

/* Forgets every texture slot. */
void ModelTexture_ResetSlots(void) {
    sent_texture_19C = 0;
    sent_texture_1A0 = 0;
    memset(texture_slots, 0, sizeof(texture_slots));
    D_00447050[0] = 0;
}

/* Sets texture slot `slot` (0..3) to chunk_table, the table ModelTexture_SendForModel then takes as sent.
   Nothing calls it. */
void ModelTexture_SetSlot(int chunk_table, u32 slot) {
    if (slot < 4U) {
        texture_slots[slot] = chunk_table;
    }
}

/* Send the textures a model needs that are not the ones already loaded. */
void ModelTexture_SendForModel(void *model_arg) {
    ModelInstance *model = model_arg;
    int changed;

    changed = FALSE;
    if (model->unk194 != 0) {
        if (texture_slots[(s16)model->unk190] != model->unk194 || model->unk178 != NULL) {
            switch ((s16)model->unk190) {
            case 0:
            case 1:
            case 2:
                texture_slots[(s16)model->unk190] = model->unk194;
                changed = TRUE;
                TextureTable_QueueEnabled((TextureTableHeader *)model->unk194, (s32)model->unk178);
                break;
            case 3:
                Map_QueuePacket((int *)model->unk194);
                break;
            }
        }
    }
    if (model->unk198 != 0) {
        if (D_00447050[model->unk192] != model->unk198 && model->unk192 == 0) {
            Map_QueuePacket((int *)model->unk198);
        }
    }
    if (model->unk19C != 0 && sent_texture_19C != model->unk19C) {
        sent_texture_19C = model->unk19C;
        Map_QueuePacket((int *)model->unk19C);
        changed = TRUE;
    }
    if (model->unk1A0 != 0 && sent_texture_1A0 != model->unk1A0) {
        sent_texture_1A0 = model->unk1A0;
        Map_QueuePacket((int *)model->unk1A0);
        changed = TRUE;
    }
    if (changed) {
        if (model->unk1BC != NULL) {
            model->unk1BC(model);
        }
        GsDraw_QueueTexFlush();
    }
}
