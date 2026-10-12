#include "kf_model.h"
#include "kf_game.h"

extern GameStatus D_0038CC50;
extern void *D_00420D50[];

/* Update handler: used by the player, it sets D_0038CC50 bit 5 and hands over
   the first word of its source. */
void func_001226E0(MapObject *object) {
    MapObject *used;

    if (D_0038CC50.bit1 && (used = D_00420D50[0]) == object) {
        D_0038CC50.bit5 = TRUE;
        D_0038CC50.unk34 = MAP_OBJECT_WORDS(used)->unk44;
    }
}
