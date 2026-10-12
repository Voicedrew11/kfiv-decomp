#include "kf_entity.h"
#include "kf_gs.h"

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_0010A718(int);
void func_00216B48(void *, void *, void *);

extern Entity *D_004467E0;
extern ModelInstance D_003BE530[];
extern char D_003E74C0[];

/* Draws each live entity's pool model again with the entity's own pose and alpha. */
void func_0019A608(void) {
    Entity *entity;
    ModelInstance *model;
    ModelPart *dst;
    ModelPart *src;
    s32 i;
    s32 count;

    func_0010A718(0);
    func_00109A50();
    func_00109A80(0x1A, 0);
    func_00109A80(0x1B, 0x248);
    func_00109A80(GS_REG_TEST_2, 0x55001);
    func_00109A80(0x4B, 1);
    func_00109AD0();
    func_00109E18();
    for (entity = D_004467E0; entity != NULL; entity = entity->next) {
        if (!ENTITY_FLAG_BITS(entity)->bit1 || entity->unk14 == NULL || (f32)entity->params->unk04 <= 0.0f) {
            continue;
        }
        if (!ENTITY_FLAG_BITS(entity)->bit17) {
            continue;
        }
        if (ENTITY_FLAG_BITS(entity)->bit30) {
            continue;
        }
        model = &D_003BE530[entity->unkD3];
        if (model->parts == NULL) {
            continue;
        }
        if (ENTITY_MODEL(entity)->part_count != model->part_count) {
            continue;
        }
        // match: each part's matrix is moved as four 128-bit rows (lq/sq)
        dst = model->parts;
        src = ENTITY_MODEL(entity)->parts;
        count = model->part_count;
        for (i = 0; i < count; i++) {
            ModelMatrixQwords *dst_matrix = (ModelMatrixQwords *)dst->unk00;
            ModelMatrixQwords *src_matrix = (ModelMatrixQwords *)src->unk00;

            dst_matrix->rows[0] = src_matrix->rows[0];
            dst_matrix->rows[1] = src_matrix->rows[1];
            dst_matrix->rows[2] = src_matrix->rows[2];
            dst_matrix->rows[3] = src_matrix->rows[3];
            dst++;
            src++;
        }
        func_00216B48(model->unk40, ENTITY_MODEL(entity)->unk40, D_003E74C0);
        model->unk148 = ENTITY_MODEL(entity)->unk148;
        model->unk14C = ENTITY_MODEL(entity)->unk14C;
        func_00109A50();
        func_00109A80(GS_REG_ALPHA_2, ((s64)(s32)(ENTITY_MODEL(entity)->alpha * 32.0f * 0.0078125f) << 32) | 0x64);
        func_00109AD0();
        model->unk1A8(model);
        func_00109E18();
    }
    func_00109A50();
    func_00109A80(0x1A, 1);
    func_00109A80(0x4B, 0);
    func_00109AD0();
}
