#include "kf_entity.h"
#include "kf_map_object.h"

extern PlayerData D_00413F80;

void func_001958B8(char *, void (*)(void), int);
void func_00138970(Entity *);
void func_00139960(Entity *);
void func_00139AE8(Entity *);
void func_00139DE0(Entity *);

void func_00139730(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = KF_MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[2] > -6000.0f && D_00413F80.position[2] < -2800.0f &&
            entity->model->position[2] >= -4800.0f) {
            player_in_zone = TRUE;
        }
        entity->prev_state = entity->state;
        if (is_open == 1) {
            if (entity->prev_state == 0) {
                entity->state = 0;
            } else {
                entity->state = 3;
            }
        } else if (player_in_zone) {
            entity->state = 2;
        } else {
            entity->state = 1;
        }
        switch (entity->state) {
        case 0:
            func_001958B8((char *)entity, (void (*)(void))func_00138970, unknown);
            break;
        case 1:
            // match: an int result keeps $2 busy after the call
            ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, (void (*)(void))func_00139960, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 2:
            // match: an int result keeps $2 busy after the call
            ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, (void (*)(void))func_00139AE8, unknown);
            unkE0->unk6A = entity->params->unk432;
            break;
        case 3:
            func_001958B8((char *)entity, (void (*)(void))func_00139DE0, unknown);
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < 4800.0f) {
        entity->model->position[0] = 4800.0f;
    }
    if (entity->model->position[0] > 6650.0f - entity->unk30) {
        entity->model->position[0] = 6650.0f - entity->unk30;
    }
    if (entity->model->position[2] > -4000.0f - entity->unk30) {
        entity->model->position[2] = -4000.0f - entity->unk30;
    }
}
