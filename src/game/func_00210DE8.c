#include "common.h"

int func_00210F88(u16 *);
int func_002110A0(u16 *);
extern u16 *D_00446FD0;
extern int D_004473E4;
extern char D_01DFB3B0[];

/* Walks the 24 entries of 0xC4C bytes; each live one is handed to the two step callbacks. */
int func_00210DE8(void) {
    u16 *entry;
    int i;
    char pad_frame[0x30]; /* unused, but retail reserves the 0x30 bytes */

    D_004473E4 = 0;
    for (i = 0; i < 0x18; i++) {
        entry = (u16 *)(D_01DFB3B0 + i * 0xC4C);
        if (*entry != 0) {
            D_00446FD0 = entry;
            while (!func_00210F88(entry)) {
                if (!func_002110A0(entry)) {
                    break;
                }
            }
        }
    }
    return D_004473E4;
}
