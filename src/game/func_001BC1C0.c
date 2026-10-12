#include "kf_map.h"

extern u8 D_00369830[];
extern TriggerTypeDef D_0041A6F8[];

void func_001DC120(int); // match: one argument is passed (func_002135A0 declares three)
void func_001CFA70(int, int, int, int, int, int, ...);

/* Adds one to the count at the index: up to 1 for indices from 150 on and for definitions with flag bit 1,
   up to 99 for the others (a message is shown past that). */
void func_001BC1C0(int index) {
    TriggerTypeDef *def;

    if (index >= 220) {
        if (D_00369830[index] == 0) {
            D_00369830[index] = 1;
        }
    } else if (index >= 150) {
        if (D_00369830[index] == 0) {
            D_00369830[index] = 1;
        }
    } else if (index >= 0) {
        def = &D_0041A6F8[index];
        if (def->flags >> 1 & 1) {
            if (D_00369830[index] == 0) {
                D_00369830[index] = 1;
                if (def->type == 3) {
                    func_001DC120(index);
                }
            }
        } else if (D_00369830[index] < 99) {
            D_00369830[index]++;
            if (def->type == 3) {
                func_001DC120(index);
            }
        } else {
            func_001CFA70(0, 0, 0, 0, 0x2D, 0x1E);
        }
    }
}
