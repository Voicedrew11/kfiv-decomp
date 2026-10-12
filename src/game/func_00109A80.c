#include "common.h"

extern u32 *D_00446754;         /* the register list being built, NULL: none */
extern int D_00446758;          /* registers in it */

u32 *func_00109990(int);

/* Adds a GS register write to the list. */
void func_00109A80(int reg, s64 value) {
    if (D_00446754 != NULL) {
        s64 *entry = (s64 *)func_00109990(1);

        entry[0] = value;
        entry[1] = reg;
        D_00446758++;
    }
}
