#include "kf_map.h"
#include "kf_player.h"

extern int D_0038A130[0x28 / 4];
extern MapSlot D_0038CD20[];
extern int D_00446770;
extern PlayerState D_00413F80;
extern s8 D_004470E3;

int func_001CBAE0(int, ...);

/* Syncs the cached byte D_004470E3 to the current map slot's value and sends it to the sound call. */
int func_001A14D8(void) {
    MapSlot *slot;
    int cached;
    int wanted;

    if (D_0038A130[3] == 0 && D_0038A130[6] == 0 && D_0038A130[1] == 0) {
        slot = &D_0038CD20[D_00446770];
        cached = D_004470E3;
        if ((u8)slot->records[(D_00413F80.map_index << 7) + 0x4D] != cached) {
            if (cached >= 0x40) {
                D_004470E3 = 0x3F;
            } else if (cached < 0) {
                D_004470E3 = 0;
            }
            wanted = (u8)slot->records[(D_00413F80.map_index << 7) + 0x4D];
            D_004470E3 = wanted;
            func_001CBAE0(1, 0xD0, 0xB81, (s8)wanted << 8);
            return func_001CBAE0(1, 0xD0, 0xC81, D_004470E3 << 8);
        }
    }
}
