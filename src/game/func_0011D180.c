#include "kf_entity.h"
#include "kf_player.h"
#include "kf_effect.h"

extern int D_0038CC80[3];
extern s32 D_004467A0;
extern int D_00446770;
extern s32 D_004467A8[2];
extern s32 D_004467B8[2];
extern MapObject D_003A1A30[];
extern MapObject D_003AFA30[];
extern void (*D_003BDA30[][16])(MapObject *);
extern void (*D_003BE3B0[])(MapObject *);
extern PlayerState D_00413F80;

float sinf(float);
float sqrtf(float);
float atan2f(float, float);
float func_0019B838(float);
int func_00116C08(int, int, int, int);

/* the objects of the current map (map 0) or the other loaded one (map 1), which the game inlines */
static inline MapObject *map_objects(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        if (D_00446770 != map) {
            return D_003AFA30;
        }
        return D_003A1A30;
    }
    return NULL;
}

static inline s32 map_object_count(s32 map) {
    if (D_004467A8[D_00446770 == map ? 0 : 1] != 0) {
        return D_004467B8[D_00446770 == map ? 0 : 1];
    }
    return 0;
}

/* Updates the map objects of both maps once a frame: remembers where each was,
   runs the step handler of its kind (D_003BDA30) and the kind's every-frame
   handler (D_003BE3B0), works out whether it is in view, and starts or stops
   the effect tied to it as it comes into or goes out of view. */
void func_0011D180(void) {
    Vector4 angles;
    int period;
    int map;
    int i;
    int count;
    MapObject *object;
    ModelInstance *model;
    ModelInstance *shown;
    MapObjectAnim *anim;
    MapRecord *record;
    MapObjectUnk08 *source;
    int kind;
    int in_view;
    f32 *from;
    f32 *next;
    f32 delta_x;
    f32 delta_y;
    f32 delta_z;

    if (D_0038CC80[0] & 8) {
        return;
    }
    period = 60; // match: the divisor is a variable
    D_004467A0 = (D_004467A0 + 1) % period;
    sinf(D_004467A0 * 6.2831855f / 60.0f); // match: the result is not used
    for (map = 0; map < 2; map++) {
        object = map_objects(map);
        count = map_object_count(map);
        for (i = 0; i < count; i++, object++) {
            if (!(object->unk10 & 0x10000)) {
                continue;
            }
            model = object->model;
            object->unk20 = model->position.x;
            object->unk24 = model->position.y;
            object->unk28 = model->position.z;
            object->unk30 = model->rotation.x;
            object->unk34 = model->rotation.y;
            object->unk38 = model->rotation.z;
            MAP_OBJECT_BITS(object)->bit2 = MAP_OBJECT_BITS(object)->bit1;
            MAP_OBJECT_BITS(object)->bit1 = FALSE;
            MAP_OBJECT_BITS_LOW(object)->bit3 = FALSE;
            if (MODEL_UNK100_BITS(model)->has_motion) {
                anim = &object->unk04->anims[MAP_OBJECT_BYTES(object)->anim];
                if (anim->unk06 == MODEL_CHANNEL(model)->param) {
                    KF_Effect_StartSpawner(anim->unk02, &model->position, &model->rotation, object);
                }
            }
            kind = (object->unk10 & 0x100000) ? 0 : object->unk04->kind;
            if (MAP_OBJECT_BYTES(object)->step < 0x10) {
                if (D_003BDA30[kind][MAP_OBJECT_BYTES(object)->step] != NULL) {
                    D_003BDA30[kind][MAP_OBJECT_BYTES(object)->step](object);
                }
            }
            if (!(object->unk10 & 0x10000)) {
                continue;
            }
            D_003BE3B0[object->unk04->kind](object);
            record = (MapRecord *)object->unk0C;
            if (record != NULL) {
                if ((MapObject *)D_00413F80.unk208 == object) {
                    object->unk10 |= 2;
                } else if (object->unk08->unk5A != 0 && object->unk08->unk5A != 0xFF) {
                    if (func_00116C08(D_00446770, D_00413F80.unk250, MAP_OBJECT_BYTES(object)->unk00 & 1, object->unk08->unk5A)) {
                        object->unk10 |= 2;
                    }
                } else if (record->unk38 & 0x4000000) {
                    object->unk10 |= 2;
                }
            }
            source = object->unk08;
            if (source->unk54 == 0) {
                continue;
            }
            in_view = object->unk10 & 6;
            if (in_view == 2) {
                shown = object->model;
                if (shown->parts != NULL) {
                    shown->unk1B4(shown);
                    from = (f32 *)object->model->get_point(object->model, object->unk08->unk56);
                    next = (f32 *)object->model->get_point(object->model, object->unk08->unk56 + 1);
                    delta_x = next[0] - from[0];
                    delta_y = next[1] - from[1];
                    delta_z = next[2] - from[2];
                    angles.x = atan2f(delta_y, sqrtf(delta_x * delta_x + delta_z * delta_z));
                    angles.y = func_0019B838(-atan2f(delta_z, delta_x) + 1.5707964f);
                    angles.z = angles.w = 0.0f;
                    KF_Effect_StartSpawner(object->unk08->unk54, from, &angles, object);
                } else {
                    KF_Effect_StartSpawner(source->unk54, &shown->position, &shown->rotation, object);
                }
            } else if (in_view == 4) {
                KF_Effect_FreeForOwner((Entity *)object);
            }
        }
    }
}
