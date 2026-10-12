#include "common.h"
#include "kf_model.h"
#include "kf_texture.h"

void *memset(void *, int, u32);
void func_00109E88(void);
void func_0010B2E0(TextureTableHeader *, s32);
void func_00116B60(int *);

static int sent_texture_19C;        /* 0x447048 last model texture sent from unk19C */
static int sent_texture_1A0;        /* 0x44704C last model texture sent from unk1A0 */
extern int D_00447050[2];           /* 0x447050 texture per unk192; [1] (0x447054) is shared, so stays extern */
static int texture_slots[4];        /* 0x468190 chunk table last sent for each unk190 slot */

/* Forgets every texture slot. */
void func_0010AE58(void) {
    sent_texture_19C = 0;
    sent_texture_1A0 = 0;
    memset(texture_slots, 0, sizeof(texture_slots));
    D_00447050[0] = 0;
}

/* Sets texture slot arg1 (0..3) to arg0. */
void func_0010AE90(int chunk_table, u32 slot) {
    if (slot < 4U) {
        texture_slots[slot] = chunk_table;
    }
}

/* Send the textures a model needs that are not the ones already loaded. */
void func_0010AEB8(void *model_arg) {
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
                func_0010B2E0((TextureTableHeader *)model->unk194, (s32)model->unk178);
                break;
            case 3:
                func_00116B60((int *)model->unk194);
                break;
            }
        }
    }
    if (model->unk198 != 0) {
        if (D_00447050[model->unk192] != model->unk198 && model->unk192 == 0) {
            func_00116B60((int *)model->unk198);
        }
    }
    if (model->unk19C != 0 && sent_texture_19C != model->unk19C) {
        sent_texture_19C = model->unk19C;
        func_00116B60((int *)model->unk19C);
        changed = TRUE;
    }
    if (model->unk1A0 != 0 && sent_texture_1A0 != model->unk1A0) {
        sent_texture_1A0 = model->unk1A0;
        func_00116B60((int *)model->unk1A0);
        changed = TRUE;
    }
    if (changed) {
        if (model->unk1BC != NULL) {
            model->unk1BC(model);
        }
        func_00109E88();
    }
}
