#include "kf_map.h"
#include "kf_game.h"

extern TriggerTypeDef D_0041A6F8[];
extern GameStatus D_0038CC50;
extern void *D_00420D50[];

void func_001CFA70(int, int, int, int, int, int, ...);
void func_001BC1C0(int);

/* Update handler of a sliding kind, step 3: used by the player, it gives back
   the item it holds and goes to step 5, unless the item is of type 5: then a
   message is shown, at most every 45 frames. */
void func_001212B0(MapObject *object) {
    MapObjectWorkSlide *work = (MapObjectWorkSlide *)object->work;
    MapObjectUnk08Words *source = MAP_OBJECT_WORDS(object);
    TriggerTypeDef *item_def = &D_0041A6F8[source->unk44];

    if (work->timer > 0) {
        work->timer--;
    }
    if (D_0038CC50.bit1 && D_00420D50[0] == object) {
        if (item_def->type == 5) {
            if (work->timer > 0) {
                return;
            }
            func_001CFA70(0, 0, 0, 0, 0x2D, (s16)object->unk04->unk68);
            work->timer = 0x2D;
        } else {
            MAP_OBJECT_BYTES(object)->step = 5;
            object->unk10 &= ~0x20000;
            func_001BC1C0(source->unk44);
            D_0038CC50.unk30 |= 1;
        }
    }
}
