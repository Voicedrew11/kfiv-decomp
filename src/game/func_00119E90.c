#include "kf_collision.h"

extern u8 D_00349F56[];

void *func_00116160(int, int);

static inline void hide_record(int map, int record_id) {
    MapRecordBounds *record = func_00116160(map, record_id);

    if (record != NULL) {
        record->unk3C |= 0x40000000;
    }
}

static inline void show_record(int map, int record_id) {
    MapRecordBounds *record = func_00116160(map, record_id);

    if (record != NULL) {
        record->unk3C &= ~0x40000000;
    }
}

/* Once the stage flag is set, swaps map records 8-12 for 0x29-0x2D. */
void func_00119E90(int map) {
    if (D_00349F56[0] != 0) {
        hide_record(map, 8);
        hide_record(map, 9);
        hide_record(map, 0xA);
        hide_record(map, 0xB);
        hide_record(map, 0xC);
        show_record(map, 0x29);
        show_record(map, 0x2A);
        show_record(map, 0x2B);
        show_record(map, 0x2C);
        show_record(map, 0x2D);
    }
}
