#include "kf_collision.h"
#include "kf_entity.h"
#include "kf_map.h"

extern u8 D_0034B696[];
extern MapSlot D_0038CD20[];

void *func_00116160(int, int);

static inline void hide_record(MapRecordBounds *record) {
    if (record != NULL) {
        record->unk3C |= 0x40000000;
    }
}

static inline void show_record(MapRecordBounds *record) {
    if (record != NULL) {
        record->unk3C &= ~0x40000000;
    }
}

/* Shows the map records that go with the stage flag (0, 1 or more) and, past 0, changes block 2 and 4 of the map. */
void func_001197F0(int map) {
    void *target; // match: one variable for every record and then the map slot; it gives the slot the records' register
    MapSlotUnk0CValues *block;
    MapSlotUnk10 *light;

    switch (D_0034B696[0]) {
    case 0:
        target = func_00116160(map, 0x33);
        show_record(target);
        target = func_00116160(map, 0x6D);
        hide_record(target);
        target = func_00116160(map, 0x6E);
        hide_record(target);
        return;
    case 1:
        target = func_00116160(map, 2);
        hide_record(target);
        target = func_00116160(map, 0x33);
        hide_record(target);
        target = func_00116160(map, 0x6D);
        show_record(target);
        target = func_00116160(map, 0x6E);
        hide_record(target);
        break;
    default:
        target = func_00116160(map, 2);
        hide_record(target);
        target = func_00116160(map, 0x33);
        hide_record(target);
        target = func_00116160(map, 0x6D);
        hide_record(target);
        target = func_00116160(map, 0x6E);
        show_record(target);
        break;
    }
    target = &D_0038CD20[map];
    block = (MapSlotUnk0CValues *)((MapSlot *)target)->unk0C;
    light = ((MapSlot *)target)->unk10;
    block[2].unk00 = block[2].unk04 = 0.0f;
    block[2].unk08 = -43.40425f;
    block[2].unk0C = 238723.4f;
    block[2].unk10 = 0;
    light[4].unk14 = 0.0f;
    light[4].unk24 = 0.0f;
    light[4].unk04 = -1.57f;
    light[4].unk50 = 0.50980395f;
    light[4].unk54 = 0.13725491f;
    light[4].unk58 = 0.0f;
    light[4].unk70 = 0.6666667f;
    light[4].unk74 = 0.38431373f;
    light[4].unk78 = 0.27450982f;
}
