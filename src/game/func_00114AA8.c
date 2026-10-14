#include "kf_map.h"
#include "kf_collision.h"
#include "kf_screen.h"
#include "kf_game.h"
#include "kf_entity.h"

extern MapSlot D_0038CD20[];
extern MapProp *D_0038CDE0[2][0xFF];
extern char map_object_defs[];
extern MapObject D_003A1A30[][0x200];
extern MapSlotMemory D_004321F0[2];
extern int D_004467B0[2];
extern s32 D_004467B8[2];

void *memset(void *, int, u32);
void func_001157D8(AreaNode *);
void func_0010D018(ModelInstance *, char **, char **);
void func_0010B758(ModelInstance *, int *, int *);
MotionData *func_00108940(MotionFile *, ModelChannel **, ModelInstance *, int);
u32 *func_00116BD0(int, int);
void func_0010E6A8(ModelInstance *, s64, s64);
void func_0010E820(ModelInstance *, s64);
MapRecordInfo *func_00115CD0(CameraState *, f32, int);
void func_00111E28(ModelInstance *, u32 *);
void func_00216778(void *, void *);
char *func_00112400(char *, ModelInstance *, int);

/* the address rounded up to a multiple of 16 */
static inline u32 align_16(u32 address) {
    u32 remainder = address & 0xF;

    if (remainder != 0) {
        remainder -= 0x10;
        address -= remainder;
    }
    return address;
}

/* Builds the models of a map that was just read: one per record, then one per map object, with
   their collision meshes and motions. */
void func_00114AA8(int map) {
    int cursor;
    char *work;
    int next;
    MapSlotMemory *memory = &D_004321F0[map];
    MapSlot *slot = &D_0038CD20[map];
    MapSlotUnk08 *header = (MapSlotUnk08 *)memory->unk1C;
    int record_count;
    MapRecordBounds *record;
    MapRecordBounds *other;
    ModelInstance *model;
    // match: the three unused initial values below leave no code, but without them two saved values swap stack slots
    ModelInstance *first = NULL;
    MapObjectUnk08 *info = NULL;
    MapObject *object = NULL;
    int i;
    int j;
    int k;

    record_count = header->record_count;
    record = (MapRecordBounds *)slot->records;
    model = (ModelInstance *)memory->unk34;
    work = memory->unk18;
    work = (char *)align_16((u32)work);
    for (i = 0; i < record_count; i++, model++, record++) {
        func_001157D8((AreaNode *)record);
        MAP_RECORD_BITS(record)->map = (u8)map;
        record->model = model;
        if (record->unk30 != 0xFFFF) {
            cursor = (int)header + header->unk04;
            k = record->unk30;
            while (k-- > 0) {
                cursor += *(int *)cursor;
            }
            func_0010D018(model, (char **)&cursor, &work);
            if (record->unk3C & 0x100) {
                func_00216778(&model->position, record);
                model->position.w = 1.0f;
                ((void (*)(ModelInstance *, u32 *, int))func_00111E28)(model, (u32 *)&work, 0); // match: a third argument the callee does not take
            }
            if (((ModelInstance **)slot->unk24)[record->unk30] == NULL) {
                ((ModelInstance **)slot->unk24)[record->unk30] = model;
            }
            func_0010E6A8(model, 0x20, 0x20);
            if (record->unk3C & 0x202) {
                func_0010E6A8(model, 0x60, 0x60);
                func_0010E820(model, 0x48);
            }
        }
        if (record->unk32 != 0xFFFF) {
            other = (MapRecordBounds *)slot->records;
            for (j = 0; j < i; j++, other++) {
                if (record->unk32 == other->unk32) {
                    break;
                }
            }
            if (j >= i) {
                cursor = (int)header + header->unk08;
                k = record->unk32;
                while (k-- > 0) {
                    cursor += (int)((CollisionMesh *)cursor)->unk14;
                }
                cursor = (int)func_00112400((char *)cursor, model, 1);
            } else {
                model->strips = ((ModelInstance *)memory->unk34)[j].strips;
            }
        }
    }
    memory->unk18 = work;
    slot->unk02 = 0;
    memset(D_0038CDE0[map], 0, sizeof(D_0038CDE0[map]));
    record = (MapRecordBounds *)slot->records;
    for (i = 0; i < record_count; i++, record++) {
        if (((MapRecord *)record)->unk4E != 0) {
            if (D_0038CDE0[map][((MapRecord *)record)->unk4E] == NULL) {
                D_0038CDE0[map][((MapRecord *)record)->unk4E] = (MapProp *)record;
            }
        }
        if ((s32)record->unk3C < 0) {
            slot->unk02 = 1;
        }
    }
    D_004467B8[map] = header->unk40;
    object = D_003A1A30[map];
    work = memory->unk18;
    model = (ModelInstance *)memory->unk3C;
    info = *(MapObjectUnk08 **)&D_004467B0[map]; // match: read as a pointer; read as an int it waits for the stores above
    for (i = 0; i < header->unk40; i++, model++, info++, object++) {
        object->model = model;
        object->unk08 = info;
        object->unk04 = (MapObjectUnk04 *)&map_object_defs[info->unk30 * 0x80];
        object->unk0C = (s32)func_00115CD0((CameraState *)info, 0.0f, map);
        MAP_OBJECT_BITS(object)->map = (u8)map;
    }
    cursor = (int)header + header->unk0C;
    for (i = 0; i < header->unk30; i++) {
        object = D_003A1A30[map];
        for (j = 0; j < header->unk40; j++, object++) {
            if (object->unk04->unk04 != 0xFFFF && !(MAP_OBJECT_DEF_WORD(object) & 0x100) && object->unk08->unk32 == i) {
                next = cursor; // match: retail bug, only set when an object uses this model (see cursor = next)
                func_0010D018(object->model, (char **)&next, &work);
                object->model->unk190 = 1;
                object->model->unk194 = next;
                next += *(int *)next;
                object->model->unk19C = 0;
                object->model->unk1A0 = 0;
            }
        }
        cursor = next; // match: retail bug, next is stale (or never set) when no object uses this model
    }
    cursor = (int)header + header->unk10;
    for (i = 0; i < header->unk32; i++) {
        object = D_003A1A30[map];
        for (j = 0; j < header->unk40; j++, object++) {
            if (object->unk04->unk04 != 0xFFFF && (MAP_OBJECT_DEF_WORD(object) & 0x100) && object->unk08->unk32 == i) {
                next = cursor; // match: retail bug, as in the loop above
                func_0010B758(object->model, &next, (int *)&work);
                object->model->unk190 = 1;
                object->model->unk194 = next;
                next += *(int *)next;
                object->model->unk19C = (int)func_00116BD0(map, 3);
                object->model->unk1A0 = (int)func_00116BD0(map, 5);
            }
        }
        cursor = next; // match: retail bug, as in the loop above
    }
    cursor = (int)header + header->unk14;
    for (i = 0; i < header->unk34; i++) {
        next = cursor;
        object = D_003A1A30[map];
        first = NULL;
        for (j = 0; j < header->unk40; j++, object++) {
            if (object->unk04->unk06 != 0xFFFF && object->unk08->unk34 == i) {
                if (first == NULL) {
                    cursor = (int)func_00112400((char *)next, object->model, 1);
                    first = object->model;
                } else {
                    object->model->strips = first->strips;
                }
            }
        }
    }
    cursor = (int)header + header->unk18;
    for (i = 0; i < header->unk36; i++) {
        object = D_003A1A30[map];
        for (j = 0; j < header->unk40; j++, object++) {
            if (object->unk04->unk04 != 0xFFFF && (MAP_OBJECT_DEF_WORD(object) & 0x200) && object->unk08->unk36 == i) {
                next = ((int (*)(int, int *, ModelInstance *, int))func_00108940)(cursor, (int *)&work, object->model, 1); // match: the callee returns where the motion ends
            }
        }
        cursor = next; // match: retail bug, as above: next is only set when an object uses this motion
    }
    memory->unk18 = work;
}
