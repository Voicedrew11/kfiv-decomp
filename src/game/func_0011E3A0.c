#include "kf_model.h"

extern s32 D_004467A4;

int func_0019D1C0(float, int, void *);
void func_0019D490(s32, s32, f32 *, f32);
void func_0019D980(char, u8, u8, f32, Vector4Arg);
void func_00108AC8(void *);
void func_0011EA80(MapObject *);

/* Advances a map object's animation one frame, starting the sounds tied to the
   frame it is on; returns whether the animation is still running. */
int func_0011E3A0(void *arg) {
    MapObject *object = arg;
    MapObjectAnim *anim = &object->unk04->anims[MAP_OBJECT_BYTES(object)->anim];
    ModelInstance *model;
    f32 range;

    if (anim->sound != 0xFFFF) {
        model = object->model;
        if (anim->sound_frame == MODEL_CHANNEL(model)->param) {
            range = 800.0f;
            if (object->unk04->unk70 > 0.0f) {
                range = object->unk04->unk70;
            }
            switch (anim->sound_kind) {
                case 0:
                    func_0019D1C0(range, anim->sound, &model->position);
                    break;
                case 1:
                    // match: the range is evaluated before the position
                    ((void (*)(s32, s32, f32, f32 *))func_0019D490)((u8)anim->sound, object->unk04->unk74, range, &model->position.x);
                    break;
                case 2:
                    // match: the vector goes by address
                    ((void (*)(char, u8, u8, f32, Vector4 *))func_0019D980)((u8)D_004467A4 + 0x20, anim->sound, object->unk04->unk74, range, &model->position);
                    object->sound = (u8)D_004467A4 + 0x20;
                    D_004467A4 = (D_004467A4 + 1) % 32;
                    break;
            }
        }
    }
    if (MAP_OBJECT_BITS(object)->anim == 4 || MAP_OBJECT_BITS(object)->anim == 5) {
        anim = &object->unk04->anims[MAP_OBJECT_BYTES(object)->anim + 2];
        if (anim->sound != 0xFFFF && anim->sound_frame == MODEL_CHANNEL(object->model)->param) {
            range = 800.0f;
            if (object->unk04->unk70 > 0.0f) {
                range = object->unk04->unk70;
            }
            func_0011EA80(object);
            switch (anim->sound_kind) {
                case 0:
                    func_0019D1C0(range, anim->sound, &object->model->position);
                    break;
                case 1:
                    func_0019D490((u8)anim->sound, object->unk04->unk74, &object->model->position.x, range);
                    break;
                case 2:
                    break;
            }
        }
    }
    func_00108AC8(object->model);
    object->model->unk1B4(object->model);
    object->unk10 |= 8;
    model = object->model;
    if (MODEL_CHANNEL(model)->param == 0) {
        func_0011EA80(object);
        model = object->model; // match: re-read here although the return reads it again; keeps the model in $6
    }
    return MODEL_CHANNEL(object->model)->param != 0;
}
