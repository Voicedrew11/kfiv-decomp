#include "kf_map_object.h"

extern s64 D_0043E780[];
void func_001C9340(int, s16, void *, void *, void *, void *);

/* Start handler of a chest: closed (animation 4, step 1) or open (animation 5, step 3);
   an open chest that isn't emptied yet drops its item (MapObjectUnk08.unk44) through func_001C9340. */
void func_0011FCF8(MapObject *object) {
    f32 drop_position[3];
    s64 drop_params[2];
    ModelInstance *model;
    MapObjectUnk08 *source;

    drop_params[0] = D_0043E780[0];
    drop_params[1] = D_0043E780[1];
    if (!(object->unk10 & 0x20000)) {
        KF_MapObject_StartAnim(object, 4);
        MAP_OBJECT_BYTES(object)->step = 1;
    } else {
        KF_MapObject_StartAnim(object, 5);
        source = object->unk08;
        MAP_OBJECT_BYTES(object)->step = 3;
        if (source->unk44 != 0 && !(object->unk10 & 0x40000)) {
            model = object->model;
            drop_position[0] = model->position.x;
            drop_position[1] = model->position.y - 25.0f;
            drop_position[2] = model->position.z;
            func_001C9340((u8)object->unk10 & 1, source->unk44, drop_position, &model->rotation, drop_params, object);
        }
    }
}
