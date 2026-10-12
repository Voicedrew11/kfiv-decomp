#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_0011E1F0(int, int);
int func_002109C8(int, int);
void func_001958B8(char *, void (*)(void), int);

extern int D_00446770;

void func_0014A058(Entity *entity) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0.0f;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 == entity->spawn_info->unk44) {
            map_object = (MapObject *)func_0011E1F0(entity->group_id, entity->spawn_info->unk43);
            if (map_object != NULL && map_object->unk08->unk40 != 0 && entity->group_id == D_00446770) {
                func_002109C8(map_object->unk08->unk40, 0);
            }
        }
        if (ENTITY_FLAG_BITS(entity)->bit34) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if (ENTITY_FLAG_BITS(entity)->bit5) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
