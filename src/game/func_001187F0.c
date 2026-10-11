#include "common.h"

extern char D_0038CD20[];
extern int D_00446770;
extern int D_0038D680[];

/* Start a transition to new_value unless one is running or it is already current. */
void func_001187F0(int new_value) {
    char *entry;

    entry = D_0038CD20 + D_00446770 * 0x60;
    if (D_0038D680[6] == FALSE && ((u8 *)entry)[1] != new_value) {
        D_0038D680[6] = TRUE;
        D_0038D680[10] = 0x1E;
        D_0038D680[7] = new_value;
        D_0038D680[9] = 0;
        D_0038D680[4] = 0;
    }
}
