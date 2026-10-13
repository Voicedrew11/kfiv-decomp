#include "kf_map_object.h"
#include "kf_model.h"

void func_00216778(void *, void *);
void func_0011F498(MapObject *);

/* Start handler of the door-like kinds: closed (animation 4) or already open
   (animation 5). Kind 6 remembers where its second part sits and, when it starts
   open, is run until it gets there. */
void func_0011F238(MapObject *object) {
    MapObjectWork *work = MAP_OBJECT_WORK(object);
    ModelPart *part;

    if (object->unk04->kind == 6) {
        MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
        func_00216778(work, object->model->parts[1].unk40);
        work->timer = 0;
        part = object->model->parts;
        work->unk10 = part[1].unk40[0];
        work->unk14 = part[1].unk40[1];
        work->unk18 = part[1].unk40[2];
        if (MAP_OBJECT_FLAGS(object) & 0x20000) {
            MAP_OBJECT_BYTES(object)->step = 2;
            do {
                func_0011F498(object);
            } while (MAP_OBJECT_BYTES(object)->step != 3);
        }
    } else {
        if (!(object->unk10 & 0x20000)) {
            MapObject_StartAnim(object, 4);
            MAP_OBJECT_BYTES(object)->step = 1;
        } else {
            MapObject_StartAnim(object, 5);
            MAP_OBJECT_BYTES(object)->step = 3;
        }
        MAP_OBJECT_UNK60_BITS(object)->bit0 = FALSE;
    }
}
