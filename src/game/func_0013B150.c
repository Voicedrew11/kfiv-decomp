#include "kf_entity.h"
#include "kf_map_object.h"

extern PlayerData D_00413F80;

float func_00194F80(void *, void *);
void func_00216778(void *, void *);
void func_001958B8(char *, void (*)(void), int);
void func_0013B4C8(Entity *, s32);
void func_0013B568(Entity *, s32);
void func_0013B608(Entity *, s32);
void func_0013B9E8(Entity *, s32);
void func_0013BA70(Entity *, s32);
void func_0013BE18(Entity *, s32);
s32 func_0013BF48(Entity *, s32);
s32 func_0013C018(Entity *, f32);
void func_00182C80(Entity *, s32);

void func_0013B150(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 post[4];
    MapObject *map_object;
    s32 first_open;
    s32 second_open;
    s32 both_open;

    first_open = 0;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->variant);
    if (map_object != NULL) {
        first_open = (map_object->unk10 >> 17) & 1;
    }
    second_open = 0;
    map_object = MapObject_GetById(entity->group_id, entity->spawn_info->unk42);
    if (map_object != NULL) {
        second_open = (map_object->unk10 >> 17) & 1;
    }
    both_open = first_open && second_open;
    if (entity->unkC6 != 0) {
        return;
    }
    if (!both_open && unkE0->unk7A != 0) {
        if (func_00194F80(entity->spawn_info, &D_00413F80) > 5400.0f) {
            func_00216778(entity->model->position, entity->spawn_info);
            unkE0->unk7A = 0;
        }
    }
    switch (unkE0->unk7A) {
    case 1:
        if (!both_open) {
            unkE0->unk7A++;
            func_001958B8((char *)entity, unkE0->unk18, unknown);
        } else {
            func_0013B4C8(entity, unknown);
        }
        break;
    case 2:
        if (!both_open) {
            if (!func_0013BF48(entity, 3) || !func_0013BF48(entity, 6) || !func_0013BF48(entity, 7)) {
                func_0013B568(entity, unknown);
            } else {
                unkE0->unk7A++;
                func_001958B8((char *)entity, unkE0->unk18, unknown);
            }
        } else {
            unkE0->unk7A = 1;
            func_001958B8((char *)entity, unkE0->unk18, unknown);
        }
        break;
    case 3:
        if (!both_open) {
            func_0013B608(entity, unknown);
        } else {
            unkE0->unk7A = 7;
            func_001958B8((char *)entity, unkE0->unk18, unknown);
        }
        break;
    case 4:
        post[0] = -18450.0f;
        post[1] = 3190.0f;
        post[2] = 4900.0f;
        unkE0->unk54 = func_00194F80(entity->model->position, post);
        unkE0->unk7A++;
        func_001958B8((char *)entity, unkE0->unk18, unknown);
        break;
    case 5:
        if (!both_open) {
            unkE0->unk7A = 0;
            func_001958B8((char *)entity, unkE0->unk18, unknown);
        } else if (func_0013BF48(entity, 3) && func_0013BF48(entity, 6) && func_0013BF48(entity, 7) &&
                   func_0013C018(entity, unkE0->unk54)) {
            unkE0->unk7A++;
            func_001958B8((char *)entity, unkE0->unk18, unknown);
        } else {
            func_0013B9E8(entity, unknown);
        }
        break;
    case 6:
        if (!both_open) {
            unkE0->unk7A = 0;
            func_001958B8((char *)entity, unkE0->unk18, unknown);
        } else {
            func_0013BA70(entity, unknown);
        }
        break;
    case 7:
        if (!both_open) {
            unkE0->unk7A = 3;
            func_001958B8((char *)entity, unkE0->unk18, unknown);
        } else {
            func_0013BE18(entity, unknown);
        }
        break;
    default:
        ENTITY_FLAG_BITS(entity)->bit27 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit20 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit23 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit26 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit21 = FALSE;
        ENTITY_FLAG_BITS(entity)->bit28 = FALSE;
        entity->unkD6 = 0x80;
        if (both_open) {
            unkE0->unk7A = 4;
            func_001958B8((char *)entity, unkE0->unk18, unknown);
        } else {
            func_00182C80(entity, unknown);
        }
        break;
    }
}
