#include "kf_entity.h"
#include "kf_map.h"
#include "kf_player.h"

extern MapSlot D_0038CD20[];
extern PlayerState D_00413F80;
extern int D_00446770;
extern int D_00447068;

/* Where the player's marker goes on the map viewer's picture D_00447068: -1 for a fixed spot on the
   first picture, TRUE for a spot worked out from the player's position, FALSE for none. */
int func_0011B1C0(int *screen_x, int *screen_y) {
    switch (D_00447068) {
    case 0:
        switch (D_0038CD20[D_00446770].tag) {
        case 0:
            if (D_00413F80.map_index != 0xFFFF
                && ((MapRecord *)D_0038CD20[D_00446770].records)[D_00413F80.map_index].unk4E == 0xC8) {
                *screen_x = 0x96;
                *screen_y = 0xDB;
            } else {
                *screen_x = 0xB6;
                *screen_y = 0x69;
            }
            return -1;
        case 2:
            if (D_00413F80.map_index != 0xFFFF
                && ((MapRecord *)D_0038CD20[D_00446770].records)[D_00413F80.map_index].unk4E == 0xC8) {
                *screen_x = 0x96;
                *screen_y = 0xDB;
            } else {
                *screen_x = 0x64;
                *screen_y = 0x151;
            }
            return -1;
        case 4:
            *screen_x = 0x214;
            *screen_y = 0x12E;
            return -1;
        case 8:
        case 9:
            *screen_x = 0x214;
            *screen_y = 0x94;
            return -1;
        case 0x62:
            *screen_x = 0x169;
            *screen_y = 0x5C;
            return -1;
        case 0x63:
            *screen_x = 0x14A;
            *screen_y = 0xFF;
            return -1;
        default:
            return FALSE;
        }
    case 1:
        if (D_00413F80.position.y > -700.0f) {
            switch (D_0038CD20[D_00446770].tag) {
            case 0x5F:
            case 0x60:
            case 0x61:
                *screen_x = (D_00413F80.position.x - 0.0f) * 24.615385f / 1000.0f + 148.0f;
                *screen_y = 273.0f - (D_00413F80.position.z - 0.0f) * 24.88889f / 1000.0f;
                return TRUE;
            }
        } else {
            switch (D_0038CD20[D_00446770].tag) {
            case 0x5F:
                if (D_00413F80.position.x > 4800.0f && D_00413F80.position.z > 8000.0f) {
                    return FALSE;
                }
            case 0x60:
                *screen_x = (D_00413F80.position.x - 3600.0f) * 24.615385f / 1000.0f + 490.0f;
                *screen_y = 143.0f - (D_00413F80.position.z - 5200.0f) * 24.88889f / 1000.0f;
                return TRUE;
            case 0x61: // match: named although it does what the default does; it shapes the compare tree
            default:
                return FALSE;
            }
        }
        break;
    }
    return FALSE;
}
