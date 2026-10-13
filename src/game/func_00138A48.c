#include "kf_entity.h"
#include "kf_map_object.h"

extern u32 D_00446ED4;
extern PlayerData D_00413F80;

void func_00196870(void *, int);
void func_001958B8(char *, void (*)(void), int);
void func_00138970(Entity *);
void func_00138D50(Entity *);
void func_00139010(Entity *);
void func_00139538(Entity *);

void func_00138A48(Entity *entity, s32 unknown) {
    EntityUnkE0 *unkE0 = &entity->unkE0;
    MapObject *map_object;
    s32 is_open;
    s32 player_in_zone;

    ENTITY_FLAG_BITS(entity)->bit18 = FALSE;
    if (D_00446ED4 % 10U == entity->unkD7 % 10U) {
        func_00196870(entity, 0);
    }
    if (entity->unkC6 == 0) {
        is_open = 0;
        map_object = KF_MapObject_GetById(entity->group_id, entity->spawn_info->variant);
        if (map_object != NULL) {
            is_open = (map_object->unk10 >> 17) & 1;
        }
        player_in_zone = FALSE;
        if (D_00413F80.position[0] > -9700.0f && D_00413F80.position[0] < -700.0f &&
            D_00413F80.position[2] > -4800.0f && D_00413F80.position[2] < -3600.0f) {
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
            // match: an int result keeps $2 busy after the call
            ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, (void (*)(void))func_00138970, unknown);
            unkE0->unk68 = entity->params->unk430;
            break;
        case 1:
            if (unkE0->unk68 == 0) {
                // match: an int result keeps $2 busy after the call
                ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, (void (*)(void))func_00138D50, unknown);
                unkE0->unk6A = entity->params->unk432;
            }
            break;
        case 2:
            if (unkE0->unk68 == 0) {
                // match: an int result keeps $2 busy after the call
                ((int (*)(char *, void (*)(void), int))func_001958B8)((char *)entity, (void (*)(void))func_00139010, unknown);
                unkE0->unk6A = entity->params->unk432;
            }
            break;
        case 3:
            if (unkE0->unk68 == 0) {
                func_001958B8((char *)entity, (void (*)(void))func_00139538, unknown);
            }
            break;
        default:
            entity->state = 0;
            break;
        }
        if (unkE0->unk68 > 0) {
            unkE0->unk68--;
        }
        if (unkE0->unk6A > 0) {
            unkE0->unk6A--;
        }
    }
    if (entity->model->position[0] < -8400.0f) {
        entity->model->position[0] = -8400.0f;
    }
    if (entity->model->position[0] > -1950.0f) {
        entity->model->position[0] = -1950.0f;
    }
    if (entity->model->position[2] > -4000.0f - entity->unk30) {
        entity->model->position[2] = -4000.0f - entity->unk30;
    }
    if (entity->model->position[0] < -3150.0f) {
        if (entity->model->position[2] < entity->unk30 + -4800.0f) {
            entity->model->position[2] = entity->unk30 + -4800.0f;
        }
    }
}
