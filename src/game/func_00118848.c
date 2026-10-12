#include "kf_map.h"
#include "kf_player.h"
#include "kf_screen.h"

extern MapSlot D_0038CD20[];
extern ScreenFade D_0038D680;
extern PlayerState D_00413F80;
extern int D_00446770;

/* Moves the player to position in the current map, or starts the change to another map. */
void func_00118848(int map, Vector4 *position) {
    MapSlot *slot = &D_0038CD20[D_00446770];

    if (D_0038D680.step == 0) {
        if (slot->tag == map) {
            if (position != NULL) {
                D_00413F80.position.x = position->x;
                D_00413F80.position.y = position->y;
                D_00413F80.position.z = position->z;
            }
        } else {
            D_0038D680.map = map;
            D_0038D680.position.x = position->x;
            D_0038D680.position.y = position->y;
            D_0038D680.position.z = position->z;
            D_0038D680.timer = 30;
            D_0038D680.step = 1;
            D_0038D680.kind = 1;
            D_0038D680.draw_step = 0;
            D_0038D680.position.w = 0.0f;
        }
    }
}
