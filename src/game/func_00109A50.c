#include "common.h"

extern u32 *D_00446754;         /* the register list being built, NULL: none */
extern int D_00446758;          /* registers in it */

u32 *func_00109990(int);

/* Starts a list of GS register writes. */
void func_00109A50(void) {
    if (D_00446754 == NULL) {
        D_00446754 = func_00109990(3);
        D_00446758 = 0;
    }
}
