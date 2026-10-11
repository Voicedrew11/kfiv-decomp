#include "common.h"

extern char D_0038CD20[];
extern int D_00446770;
extern float D_00413F80[];

/* Map the player position to map-screen coordinates; FALSE if the area has no such map. */
int func_0011BAB8(int *map_x, int *map_y) {
    int area;
    char *entry;

    entry = D_0038CD20 + D_00446770 * 0x60;
    area = ((u8 *)entry)[1];
    if (area >= 0x11) {
        if (area < 0x14) {
            *map_x = (D_00413F80[0] - 0.0f) * 22.857143f / 1000.0f + 360.0f;
            *map_y = 206.0f - (D_00413F80[2] - 0.0f) * 22.4f / 1000.0f;
            return TRUE;
        }
        if (area < 0x1A) {
            if (area >= 0x18) {
                *map_x = (D_00413F80[0] - 0.0f) * 22.857143f / 1000.0f + 205.0f;
                *map_y = 368.0f - (D_00413F80[2] - 0.0f) * 22.4f / 1000.0f;
                return TRUE;
            }
        }
    }
    return FALSE;
}
