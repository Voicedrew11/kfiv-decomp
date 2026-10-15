#include "kf_map_object.h"
#include "kf_model.h"

extern int D_00446770;
extern int D_0038A130[0x28 / 4];

s32 Sound_StartTaggedAt(s8, u8, u8, f32, Vector4Arg *);

/* Update handler: keeps a kind 0 object's sound playing while its map is the
   current one, and runs its animation when it is allowed to. */
void func_0011EE78(MapObject *object) {
    MapObjectUnk08 *source;

    if (object->unk04->kind == 0) {
        source = object->unk08;
        if ((u32)((u16)source->unk44 - 0x40) < 0x40) {
            if (object->work[0] != 0) {
                if ((MAP_OBJECT_BYTES(object)->unk00 & 1) != D_00446770) {
                    object->work[0] = 0;
                }
            } else if ((MAP_OBJECT_BYTES(object)->unk00 & 1) == D_00446770 && D_0038A130[4] == 0 && D_0038A130[5] == 0) {
                // match: the vector goes by address
                ((int (*)(char, u8, u8, f32, Vector4 *))Sound_StartTaggedAt)(source->unk44, source->unk48, source->unk46, source->unk4A, &object->model->position);
                object->work[0] = 1;
            }
        }
    }
    if (object->model->unk100 & 1) {
        if (object->unk10 & 0x100000) {
            if (object->unk10 & 0x200000) {
                if ((object->unk10 & 0x400000) || MODEL_CHANNEL(object->model)->param != 0) {
                    MapObject_StepAnim(object);
                }
            }
        } else if (object->unk08->unk38.flags & (0x8000L << 35)) {
            MapObject_StepAnim(object);
        }
    }
}
