#include "common.h"
#include "model/model.h"
#include "gfx/texture.h"
#include "core/file.h"
#include "gfx/gs_draw.h"
#include "map/map.h"

static int sent_texture_19C;        /* 0x447048 last model texture sent from unk19C */
static int sent_texture_1A0;        /* 0x44704C last model texture sent from unk1A0 */
/* 0x447050 the model texture taken as sent from unk198, by unk192; only [0] is read, and nothing but the reset
   writes it. A 4-byte object: [1] would be screen_uv_frame_copy (model/model_screen.c), so it stays extern. */
static int texture_slots[4];        /* 0x468190 chunk table last sent for each unk190 slot */

/* Forgets every texture slot. */
void ModelTexture_ResetSlots(void) {
    sent_texture_19C = 0;
    sent_texture_1A0 = 0;
    memset(texture_slots, 0, sizeof(texture_slots));
    shared_texture_packet_sent[0] = 0;
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
    if (model->texture_data != 0) {
        if (texture_slots[(s16)model->texture_slot] != model->texture_data || model->chunk_skip_mask != NULL) {
            switch ((s16)model->texture_slot) {
            case 0:
            case 1:
            case 2:
                texture_slots[(s16)model->texture_slot] = model->texture_data;
                changed = TRUE;
                TextureTable_QueueEnabled((TextureTableHeader *)model->texture_data, (s32)model->chunk_skip_mask);
                break;
            case 3:
                Map_QueuePacket((int *)model->texture_data);
                break;
            }
        }
    }
    if (model->shared_texture_packet != 0) {
        if (shared_texture_packet_sent[model->shared_texture_slot] != model->shared_texture_packet && model->shared_texture_slot == 0) {
            Map_QueuePacket((int *)model->shared_texture_packet);
        }
    }
    if (model->texture_packet_3 != 0 && sent_texture_19C != model->texture_packet_3) {
        sent_texture_19C = model->texture_packet_3;
        Map_QueuePacket((int *)model->texture_packet_3);
        changed = TRUE;
    }
    if (model->texture_packet_5 != 0 && sent_texture_1A0 != model->texture_packet_5) {
        sent_texture_1A0 = model->texture_packet_5;
        Map_QueuePacket((int *)model->texture_packet_5);
        changed = TRUE;
    }
    if (changed) {
        if (model->on_textures_sent != NULL) {
            model->on_textures_sent(model);
        }
        GsDraw_QueueTexFlush();
    }
}
