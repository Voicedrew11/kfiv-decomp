#include "kf_map.h"
#include "kf_player.h"

extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;
extern int D_00446770;

/* Turns the player's position into a point on the map picture of maps 0x1A and 0x1B; FALSE on other maps. */
int func_0011BBC0(int *screen_x, int *screen_y) {
    switch (D_0038CD20[D_00446770].tag) {
    case 0x1A:
        *screen_x = (D_00413F80.position.x - -3200.0f) * 21.333334f / 1000.0f + 438.0f;
        *screen_y = 214.0f - (D_00413F80.position.z - -5600.0f) * 20.363636f / 1000.0f;
        break;
    case 0x1B:
        *screen_x = (D_00413F80.position.x - -8800.0f) * 21.333334f / 1000.0f + 320.0f;
        *screen_y = 82.0f - (D_00413F80.position.z - 900.0f) * 20.363636f / 1000.0f;
        break;
    default:
        return FALSE;
    }
    return TRUE;
}
