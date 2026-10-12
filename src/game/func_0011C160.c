#include "kf_map.h"
#include "kf_gs.h"
#include "kf_texture.h"

extern MapSlot D_0038CD20[];
extern MapDrawState D_0038A1B0;
extern MapSlotUnk10Words D_0038D930;
extern char D_0038D8B0[0x40];
extern char D_0038D8F0[0x40];
extern s64 D_00432E20[];
extern s64 D_00433050[];
extern u8 D_00349680[0x10];
extern char *D_0044705C;
extern int D_0044706C;
extern ModelInstance D_00472080;

float sinf(float);
void func_00106C58(void);
void func_00107CD8(int);
void func_00107E70(int, int, int);
void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_0010A100(void *);
void func_0010B410(TextureTableHeader *, s32);
void func_0010D308(ModelInstance *);

/* Draws the map viewer: the pointer model, pulsing, then every record of the
   ordinary maps in one flat colour, then the viewer's four pictures. */
void func_0011C160(void) {
    int alpha = 0x10;
    int period;
    int map;
    int i;
    MapSlot *slot;
    MapDrawRecord *record;
    int flags;

    D_00432E20[0] = 0;
    D_00432E20[30] = 0;
    D_00433050[0] = 0;
    D_00433050[30] = 0;
    if (D_0044706C < 45) {
        alpha = (int)(sinf(D_0044706C * 3.1415927f / 45.0f) * 108.0f) + 0x10;
    }
    period = 60; // match: the divisor is a variable
    D_0044706C = (D_0044706C + 1) % period;
    func_00109A50();
    func_00109A80(GS_REG_TEST_2, 0x51001);
    func_00109A80(GS_REG_ALPHA_2, ((s64)alpha << 32) | 0x68);
    func_00109AD0();
    D_00472080.unk148 = (s32)&D_0038D930;
    D_00472080.hit_count = 0;
    D_00472080.unk14C = &D_0038A1B0.unkA0;
    func_0010D308(&D_00472080);
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
            switch (slot->tag) {
                case 0:
                case 2:
                case 4:
                case 8:
                case 9:
                case 0x62:
                case 0x63:
                    break;
                default:
                    for (i = 0; i < ((MapSlotUnk08 *)slot->header)->record_count; i++, record++) {
                        flags = record->flags;
                        if (flags & 0x40000000) {
                            continue;
                        }
                        if (flags & 0x20000000) {
                            continue;
                        }
                        if (flags < 0) {
                            continue;
                        }
                        if (flags & 0x10) {
                            record->model->unk148 = (s32)&D_0038A1B0.unkC0;
                            record->model->unk14C = &D_0038A1B0.unkA0;
                        } else {
                            record->model->unk148 = (s32)&D_0038A1B0.unkA90[map][record->unk35];
                            record->model->unk14C = &D_0038A1B0.unk290[map][record->unk34];
                        }
                        record->model->hit_count = 0;
                        record->model->unk148 = (s32)&D_0038D930;
                        record->model->unk14C = &D_0038A1B0.unkA0;
                        func_0010D308(record->model);
                    }
                    break;
            }
        }
    }
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_2, 0xA000118);
    func_00109A80(0x1A, 1);
    func_00109AD0();
    func_00109E18();
    func_00106C58();
    func_00109E18();
    func_00107E70(0xEA, 0xB2, 0x46);
    func_0010B410((TextureTableHeader *)D_0044705C, 0);
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x48);
    func_00109A80(8, 5);
    func_00109AD0();
    func_00109E18();
    func_0010A100(D_0038D8B0);
    func_0010B410((TextureTableHeader *)D_0044705C, 1);
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x54);
    func_00109A80(8, 5);
    func_00109AD0();
    func_00109E18();
    func_0010A100(D_0038D8B0);
    func_0010B410((TextureTableHeader *)D_0044705C, 2);
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x44);
    func_00109A80(8, 0);
    func_00109AD0();
    func_00109E18();
    func_0010A100(D_0038D8F0);
    func_0010B410((TextureTableHeader *)D_0044705C, 3);
    func_00109E88();
    func_00109E18();
    func_00109A50();
    func_00109A80(GS_REG_TEST_1, 0x31001);
    func_00109A80(GS_REG_ALPHA_1, 0x48);
    func_00109AD0();
    func_00109E18();
    func_0010A100(D_0038D8F0);
    func_00107CD8(D_00349680[10]);
}
