#include "kf_gs.h"
#include "kf_map.h"
#include "kf_packet.h"
#include "kf_texture.h"

extern MapDrawState D_0038A1B0;
extern MapSlotUnk10Words D_0038CCA0;
extern MapSlot D_0038CD20[];
extern s64 D_00432E20[];
extern s64 D_00433050[];
extern TextureTableHeader *D_00446790;

void *memset(void *, int, u32);
void func_00106C58(void);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_0010A100(void *);
void func_0010B238(TextureTableHeader *);
void func_0010D308(ModelInstance *);

/* Draws every record of both map slots with the viewer's own block, then four quarter-screen rectangles
   on top. */
void func_00116610(void) {
    ScreenQuad quad;
    MapSlot *slot;
    MapDrawRecord *record;
    int map;
    int i;

    func_0010B238(D_00446790);
    func_00109E88();
    D_00432E20[0] = 0;
    D_00432E20[30] = 0;
    D_00433050[0] = 0;
    D_00433050[30] = 0;
    func_00109A50();
    func_00109A80(0x1A, 0);
    func_00109A80(0x1B, 0x248);
    func_00109A80(GS_REG_ALPHA_2, 0x4000000068L);
    func_00109A80(GS_REG_TEST_2, 0x3080B);
    func_00109A80(GS_REG_ZBUF_2, 0x10A000118L);
    func_00109AD0();
    func_00109E18();
    for (map = 0; map < 2; map++) {
        slot = &D_0038CD20[map];
        record = (MapDrawRecord *)slot->records;
        if (slot->unk00 != 0) {
            for (i = 0; i < ((MapSlotUnk08 *)slot->header)->record_count; i++, record++) {
                if (!(record->flags & 0x80000000)) {
                    if (record->flags & 0x10) {
                        record->model->unk148 = (s32)&D_0038A1B0.unkC0;
                        record->model->unk14C = &D_0038A1B0.unkA0;
                    } else {
                        record->model->unk148 = (s32)&D_0038A1B0.unkA90[map][record->unk35];
                        record->model->unk14C = &D_0038A1B0.unk290[map][record->unk34];
                    }
                    record->model->hit_count = 0;
                    // match: both blocks chosen above are replaced at once by the viewer's own
                    record->model->unk148 = (s32)&D_0038CCA0;
                    record->model->unk14C = &D_0038A1B0.unkA0;
                    func_0010D308(record->model);
                }
            }
        }
    }
    func_00106C58();
    func_00109A50();
    func_00109A80(0x1A, 1);
    func_00109A80(GS_REG_ZBUF_2, 0xA000118);
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x48);
    func_00109A80(8, 5);
    func_00109AD0();
    func_00109E18();
    memset(&quad, 0, sizeof(quad));
    quad.tag = 0x4007A80621312E40L;
    quad.unk08 = 0x60;
    quad.unk10 = 0x156;
    quad.red = 0x80;
    quad.green = 0x80;
    quad.blue = 0x80;
    quad.alpha = 0x40;
    quad.first_x = 0x6C00;
    quad.first_y = 0x7200;
    quad.first_z = 0xFFFFFFFF;
    quad.first_u = 0;
    quad.first_v = 0;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    quad.second_z = 0xFFFFFFFF;
    quad.second_u = 0x1000;
    quad.second_v = 0x1000;
    func_0010A100(&quad);
    quad.first_x = 0x9400;
    quad.first_y = 0x7200;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    func_0010A100(&quad);
    quad.first_x = 0x6C00;
    quad.first_y = 0x8E00;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    func_0010A100(&quad);
    quad.first_x = 0x9400;
    quad.first_y = 0x8E00;
    quad.second_x = 0x8000;
    quad.second_y = 0x8000;
    func_0010A100(&quad);
    func_00109A50();
    func_00109A80(8, 0);
    func_00109AD0();
    func_00109E18();
}
