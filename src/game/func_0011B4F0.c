#include "kf_map.h"
#include "kf_player.h"

extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;
extern int D_00446770;
extern int D_00447068;

/* Where the player's marker goes on the map viewer's picture D_00447068 (0 to 2): TRUE with a spot worked
   out from the player's position when the current map is on that picture, FALSE when it is not. */
int func_0011B4F0(int *screen_x, int *screen_y) {
    switch (D_00447068) {
    case 0:
        switch (D_0038CD20[D_00446770].tag) {
        case 0x70:
        case 0x71:
            *screen_x = (D_00413F80.position.x - 0.0f) * 18.626263f / 1000.0f + 321.0f;
            *screen_y = 280.0f - (D_00413F80.position.z - 0.0f) * 18.666666f / 1000.0f;
            return TRUE;
        case 0x73:
            *screen_x = (D_00413F80.position.x - -10500.0f) * 18.626263f / 1000.0f + 321.0f;
            *screen_y = 280.0f - (D_00413F80.position.z - 500.0f) * 18.666666f / 1000.0f;
            return TRUE;
        case 0x74:
            *screen_x = (D_00413F80.position.x - -9700.0f) * 18.626263f / 1000.0f + 321.0f;
            *screen_y = 280.0f - (D_00413F80.position.z - -8900.0f) * 18.666666f / 1000.0f;
            return TRUE;
        }
        break;
    case 1:
        switch (D_0038CD20[D_00446770].tag) {
        case 0x1F:
            *screen_x = (D_00413F80.position.x - 0.0f) * 11.243243f / 1000.0f + 348.0f;
            *screen_y = 263.0f - (D_00413F80.position.z - -12100.0f) * 11.48718f / 1000.0f;
            return TRUE;
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F:
            *screen_x = (D_00413F80.position.x - 0.0f) * 11.243243f / 1000.0f + 348.0f;
            *screen_y = 263.0f - (D_00413F80.position.z - 0.0f) * 11.48718f / 1000.0f;
            return TRUE;
        case 0x73:
            *screen_x = (D_00413F80.position.x - -10500.0f) * 11.243243f / 1000.0f + 348.0f;
            *screen_y = 263.0f - (D_00413F80.position.z - 500.0f) * 11.48718f / 1000.0f;
            return TRUE;
        }
        break;
    case 2:
        switch (D_0038CD20[D_00446770].tag) {
        case 0xF:
            *screen_x = (D_00413F80.position.x - 9300.0f) * 11.186992f / 1000.0f + 194.0f;
            *screen_y = 167.0f - (D_00413F80.position.z - -4400.0f) * 11.2f / 1000.0f;
            return TRUE;
        case 0x10:
            *screen_x = (D_00413F80.position.x - 700.0f) * 11.186992f / 1000.0f + 335.0f;
            *screen_y = 328.0f - (D_00413F80.position.z - 1650.0f) * 11.2f / 1000.0f;
            return TRUE;
        case 0x64:
        case 0x69:
        case 0x6A:
            *screen_x = (D_00413F80.position.x - 0.0f) * 11.186992f / 1000.0f + 326.0f;
            *screen_y = 168.0f - (D_00413F80.position.z - 0.0f) * 11.2f / 1000.0f;
            return TRUE;
        case 0x65:
            *screen_x = (D_00413F80.position.x - 3800.0f) * 11.186992f / 1000.0f + 326.0f;
            *screen_y = 168.0f - (D_00413F80.position.z - -7900.0f) * 11.2f / 1000.0f;
            return TRUE;
        case 0x66:
        case 0x67:
        case 0x68:
            *screen_x = (D_00413F80.position.x - -7050.0f) * 11.186992f / 1000.0f + 326.0f;
            *screen_y = 168.0f - (D_00413F80.position.z - 0.0f) * 11.2f / 1000.0f;
            return TRUE;
        }
        break;
    }
    return FALSE;
}
