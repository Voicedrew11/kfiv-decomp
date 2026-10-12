#include "kf_map.h"
#include "kf_file.h"

extern MapLoadState D_0038D610;
extern MapSlotMemory D_004321F0[2];
extern int D_00446770;

void func_00102228(void);

/* Map load step: reads the next 0x80000 bytes of the map file, closing it at the end. */
void func_00118458(void) {
    MapSlotMemory *memory = &D_004321F0[0];

    if (D_00446770 == 0) {
        memory = &D_004321F0[1];
    }
    if (func_001025C0(D_0038D610.file_handle, memory->unk14, 0x80000) > 0) {
        memory->unk14 += 0x80000;
        D_0038D610.step++;
    } else {
        func_001024B0(D_0038D610.file_handle);
        func_00102228();
        D_0038D610.file_handle = -1;
        memory->unk14 = memory->unk1C;
        D_0038D610.step += 2;
    }
}
