#include "common.h"

extern u8 recollection_pages[];

/* Script command: Recollection page `command[2]` shows its next description (3 at most), then finishes. */
void func_00212008(int *command) {
    u32 page;
    u8 *state;

    page = command[2];
    if (page < 0x32U) {
        state = recollection_pages + page;
        if (*state < 3U) {
            *state = *state + 1;
        }
    }
    command[0] = 0;
}
