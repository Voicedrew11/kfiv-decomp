#include "kf_map.h"
#include "kf_gs.h"
#include "kf_map_scroll.h"

extern MapSlot D_0038CD20[];
extern MapDrawState D_0038A1B0;
extern MapDrawRecord *D_00446780[2];
extern int D_00446770;

void func_00109A50(void);
void func_00109A80(int, s64);
void func_00109AD0(void);
void func_00109E18(void);
void func_00109E88(void);
void func_0010A008(void);
void func_0010EE98(ModelInstance *);
void func_00111368(ModelInstance *, int, int, int, int);
void func_00116970(void);
void func_00116AD8(int, int);
int func_00116CB0(MapDrawRecord *, ModelInstance *);
void func_0019AEF8(MapDrawRecord *, ModelInstance *);

/* Draws the second list of records of both map slots, switching z writes on and off as the
   records ask. mode 0 draws every record, the others only some; below 2 the screen is cleared first. */
void func_00114528(int mode) {
    MapSlot *slot;
    MapDrawRecord *record;
    MapSlotUnk08 *header;
    int map;
    int active;
    int flags;
    int z_mask;
    int old_z_mask;

    active = 0;
    if (D_0038CD20[0].unk00 != 0) {
        active = D_0038CD20[0].unk02 != 0;
    }
    if (D_0038CD20[1].unk00 != 0) {
        if (D_0038CD20[1].unk02 != 0) {
            active++;
        }
    }
    for (map = 0; map < 2; map++) {
        record = D_00446780[map];
        slot = &D_0038CD20[map];
        if (slot->unk00 != 0) {
            func_00116AD8(map, 2);
            func_00116AD8(map, 5);
            func_00109E88();
            KF_MapScroll_UpdateDraw(map, 1);
            KF_MapScroll_UpdateCopy(map, 1);
            if (mode < 2) {
                func_0010A008();
            } else {
                func_00116970();
            }
            func_00109A50();
            z_mask = 1;
            func_00109A80(GS_REG_TEST_1, 0x5000D);
            func_00109A80(GS_REG_ZBUF_1, 0x10A000118L);
            func_00109AD0();
            for (; record != NULL; record = record->next) {
                flags = record->flags;
                if ((flags & 2) && mode != 0) {
                    continue;
                }
                if ((flags & 0x200) && mode != 0) {
                    continue;
                }
                if (!(flags & 0x20) && mode != 0) {
                    continue;
                }
                if (active >= 2 && flags < 0 && map != D_00446770) {
                    continue;
                }
                old_z_mask = z_mask;
                z_mask = !((flags >> 7) & 1);
                if (z_mask != old_z_mask) {
                    func_00109A50();
                    func_00109A80(GS_REG_ZBUF_1, ((s64)z_mask << 32) | 0xA000118);
                    func_00109AD0();
                }
                if (record->flags & 0x10) {
                    record->model->unk148 = (s32)&D_0038A1B0.unkC0;
                    record->model->unk14C = &D_0038A1B0.unkA0;
                } else {
                    record->model->unk148 = (s32)&D_0038A1B0.unkA90[map][record->unk35];
                    record->model->unk14C = &D_0038A1B0.unk290[map][record->unk34];
                }
                func_0019AEF8(record, record->model);
                if (record->flags & 0x200) {
                    header = slot->header;
                    func_00111368(record->model, header->unk78, header->unk79, header->unk7A, header->unk7B);
                }
                record->model->unk1A8(record->model);
                if (func_00116CB0(record, record->model) == 0) {
                    func_0010EE98(record->model);
                }
                func_00109E18();
            }
        }
    }
    func_00109A50();
    func_00109A80(GS_REG_ZBUF_1, 0xA000118);
    func_00109AD0();
}
