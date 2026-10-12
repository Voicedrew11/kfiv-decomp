#include "kf_map.h"
#include "kf_collision.h"

extern s32 D_004467B8[2];
extern MapObject D_003A1A30[][0x200];
extern MapObject *D_00472240[][0x100];
extern MapObject *D_00447088[2];
extern MapObject *D_00447090[2];
extern MapSlot D_0038CD20[];
extern void (*D_003BE3B0[])(MapObject *);

void func_0010E010(ModelInstance *);
void func_0011E8E8(int *);
void func_0011E8F0(ModelInstance *);
void func_0011E9D0(void *);
void func_0011EA70(void);
char *func_0011EA78(char *);
void func_0011EAC0(void);
void func_0011ED18(void);
void func_00216778(void *, void *);
void func_0020C300(ModelInstance *, Vector4 *, Vector4 *, f32 *);
void func_0020B8B0(ModelInstance *, ModelPart *, ModelPart *);

/* Sets up the objects of a map once it is loaded: puts each model where its
   source says, works out how far the object reaches, picks the functions that
   draw it, runs the every-frame handler of its kind once, and threads the
   objects into the two draw lists of the map, grouped by the model they use. */
void func_0011C708(int map) {
    int i = 0;
    int count = D_004467B8[map];
    MapObject *object = D_003A1A30[map];
    f32 scale;
    int link_id;
    int j;
    int part;

    for (; i < count; i++, object++) {
        MAP_OBJECT_BYTES(object)->step = 0;
        if (object->unk04->unk04 == 0xFFFF) {
            object->model->get_point = (int (*)(void *, int))func_0011EA78;
            object->model->unk1B4 = (void (*)(ModelInstance *))func_0011EA70;
        }
        if (object->unk08->unk38.bytes[2] != 0) {
            link_id = object->unk08->unk38.bytes[2];
            if (D_00472240[map][link_id] == NULL) {
                D_00472240[map][link_id] = object;
            }
        }
        func_00216778(&object->model->position, &object->unk08->position);
        func_00216778(&object->model->rotation, &object->unk08->rotation);
        func_00216778(&object->model->unkE0, &object->unk08->scale);
        object->unk20 = object->model->position.x;
        object->unk24 = object->model->position.y;
        object->unk28 = object->model->position.z;
        object->unk30 = object->model->rotation.x;
        object->unk34 = object->model->rotation.y;
        object->unk38 = object->model->rotation.z;
        scale = ABS(object->unk08->scale.x) > ABS(object->unk08->scale.z) ? ABS(object->unk08->scale.x) : ABS(object->unk08->scale.z);
        if (object->unk04->unk0C > 0.0f) {
            object->unk14 = object->unk04->unk0C * scale + 100.0f;
        } else if (object->model->strips != NULL) {
            object->unk14 = ((CollisionMesh *)object->model->strips)->unk10 * scale + 100.0f;
        } else {
            object->unk14 = 0.0f;
        }
        object->unk3C = object->unk04->unk6C * scale + 100.0f;
        object->model->unk13C = object->unk14;
        if (object->model->unk100 & 1) {
            MAP_OBJECT_BYTES(object)->anim = 0;
        } else {
            MAP_OBJECT_BYTES(object)->anim = -1;
            func_0020C300(object->model, &object->model->position, &object->model->rotation, &object->model->unkE0);
            for (j = 0; j < object->model->part_count; j++) {
                func_0020B8B0(object->model, &object->model->parts[j], &object->model->parts[j]);
            }
        }
        D_003BE3B0[object->unk04->kind](object);
        switch (object->unk04->bit8) {
            case 0:
                object->model->draw = (void (*)(ModelInstance *))func_0011E8E8;
                object->model->unk1A8 = func_0010E010;
                break;
            case 1:
                object->model->draw = func_0011E8F0;
                break;
            default:
                object->model->draw = NULL;
                break;
        }
        if (object->unk04->kind == 5) {
            object->model->draw = (void (*)(ModelInstance *))func_0011E9D0;
        }
        if (object->unk08->unk30 == 0x91) {
            object->model->unk1BC = (void (*)(ModelInstance *))func_0011ED18;
            func_0011EAC0();
        }
        MAP_OBJECT_BITS(object)->bit1 = FALSE;
        MAP_OBJECT_BITS(object)->bit2 = FALSE;
    }
    D_00447088[map] = NULL;
    for (i = ((MapSlotUnk08 *)D_0038CD20[map].unk08)->unk30 - 1; i >= 0; i--) {
        for (object = &D_003A1A30[map][count - 1], j = count - 1; j >= 0; j--, object--) {
            if (MAP_OBJECT_DEF_WORD(object) & 0x400) {
                continue;
            }
            if (MAP_OBJECT_DEF_WORD(object) & 0x100) {
                continue;
            }
            if (object->unk08->unk32 != i) {
                continue;
            }
            object->unk1C = D_00447088[map];
            D_00447088[map] = object;
        }
    }
    D_00447090[map] = NULL;
    for (i = ((MapSlotUnk08 *)D_0038CD20[map].unk08)->unk32 - 1; i >= 0; i--) {
        for (object = &D_003A1A30[map][count - 1], j = count - 1; j >= 0; j--, object--) {
            if (MAP_OBJECT_DEF_WORD(object) & 0x400) {
                continue;
            }
            if (!(MAP_OBJECT_DEF_WORD(object) & 0x100)) {
                continue;
            }
            if (object->unk08->unk32 != i) {
                continue;
            }
            object->unk1C = D_00447090[map];
            D_00447090[map] = object;
        }
    }
}
