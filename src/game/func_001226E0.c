#include "kf_model.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;
extern void *D_00420D50[];

/* Update handler of the save point (kind 20): used by the player, it asks for the save screen (GameStatus.bit5)
   and hands over its word 0x44, the message id of the area's name that the save list shows. */
void MapObject_UpdateSavePoint(MapObject *object) {
    MapObject *used;

    if (D_0038CC50.bit1 && (used = D_00420D50[0]) == object) {
        D_0038CC50.save_requested = TRUE;
        D_0038CC50.save_area = MAP_OBJECT_WORDS(used)->unk44;
    }
}
