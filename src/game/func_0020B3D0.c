#include "kf_map.h"

extern char D_005FAF80[];
extern MapSlotMemory D_004321F0[2];
extern MapSlotMemory *D_00446F08;
extern int D_00446F0C;
extern int D_00446F10;
extern int D_00446F14;
extern int D_00446F18;
extern int D_00446F1C;
extern int D_00446F20;
extern int D_00446F24;
extern int D_00446F28;
extern char *D_00446F50;
extern char *D_00446F54;
extern char *D_00446F58;
extern char *D_00446F5C;
extern char *D_00446F60;
extern char *D_00446F64;
extern char *D_00446F68;
extern char *D_00446F6C;
extern char *D_00446F70;
extern char *D_00446F74;

/* Places the memory areas one after another from D_005FAF80, using the sizes func_0020B208 set. */
void func_0020B3D0(void) {
    char *next_area = D_005FAF80;

    D_00446F08 = D_004321F0;
    D_00446F58 = next_area;
    next_area += D_00446F0C;
    D_00446F68 = D_00446F58;
    D_00446F5C = next_area;
    next_area += D_00446F10;
    D_00446F60 = next_area;
    next_area += D_00446F14;
    D_00446F64 = next_area;
    next_area += D_00446F18;
    D_00446F50 = next_area;
    next_area += D_00446F20;
    D_00446F54 = next_area;
    next_area += D_00446F1C;
    D_00446F6C = next_area;
    next_area += D_00446F24;
    D_00446F70 = next_area;
    next_area += D_00446F28;
    D_00446F74 = next_area;
}
