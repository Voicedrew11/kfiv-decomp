#include "kf_entity.h"

int func_00192C98(void *, int, int, u32, int);
int func_0011E1F0(int, int);
void func_0011D660(MapObject *, int);
void func_001958B8(char *, void (*)(void), int);

void func_00178E10(Entity *entity) {
    EntityUnkE0Kind178358 *unkE0 = (EntityUnkE0Kind178358 *)&entity->unkE0;
    MapObject *map_object;

    entity->unkC6 |= 0x1000;
    switch (entity->action_step) {
    case 0:
        func_00192C98(entity, 0x11, 0, entity->params->unkC4, 0);
        ENTITY_FLAG_BITS(entity)->bits11 = 1;
        unkE0->unk1C = 0;
        entity->action_step++;
        break;
    case 1:
        if (entity->model->unkFC->unk88 == 0) {
            entity->action_step++;
        }
        break;
    case 2:
        if (entity->model->unkFC->unk82 == 20) {
            map_object = (MapObject *)func_0011E1F0(entity->group_id, unkE0->unk7C);
            if (map_object != NULL) {
                func_0011D660(map_object, 1);
            }
        }
        if ((s32)(entity->flags >> 34) & 1) {
            entity->unkC6 = 0;
            entity->unkC8 = 0;
            entity->unkA0 = 0;
            if ((s32)(entity->flags >> 5) & 1) {
                unkE0->unk00(entity, 1);
            } else {
                func_001958B8((char *)entity, unkE0->unk18, 1);
            }
        }
        break;
    }
}
