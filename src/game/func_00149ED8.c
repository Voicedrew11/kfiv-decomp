#include "kf_entity.h"

int func_0011E1F0(int, int);
void func_001958B8(char *, void (*)(void), int);
float func_00194F80(void *, void *);
void func_00216778(void *, void *);
s16 func_00190BE0(Entity *);
s16 func_00190F90(float, void *, int);
void func_0014A058(Entity *);

void func_00149ED8(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    f32 stand_point[4];
    f32 face_point[4];
    f32 reach;
    MapObject *map_object;

    entity->flags &= ~0x200;
    ENTITY_FLAG_BITS(entity)->bits11 = 1;
    map_object = (MapObject *)func_0011E1F0(entity->group_id, entity->spawn_info->unk43);
    if (map_object != NULL) {
        stand_point[0] = map_object->model->position.x;
        stand_point[1] = map_object->model->position.y;
        stand_point[2] = map_object->model->position.z + 100.0f;
        stand_point[3] = 0.0f;
        face_point[0] = map_object->model->position.x;
        face_point[1] = map_object->model->position.y;
        face_point[2] = map_object->model->position.z;
        face_point[3] = 0.0f;
    }
    reach = 100.0f;
    if (func_00194F80(entity->model->position, stand_point) < reach) {
        ENTITY_FLAG_BITS(entity)->bits11 = 0;
        func_00216778(unkE0->unk30, face_point);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190BE0(entity);
        if (entity->unkC8 == 0) {
            func_001958B8((char *)entity, (void (*)(void))func_0014A058, unknown);
            return;
        }
    } else {
        func_00216778(unkE0->unk30, stand_point);
        entity->target = unkE0->unk30;
        entity->unkC8 = func_00190F90(reach * 0.5f, entity, 0);
    }
    func_001958B8((char *)entity, unkE0->unk18, unknown);
}
