#include "kf_entity.h"

extern float D_00413F80[];

int func_00123AF0(int type_id);
int func_00123B40(void);
int func_00123C30(void);
int func_00123DD8(int pool_size);
int func_00123ED8(int pool_size);
int func_00123918(Entity *entity);
void func_00124318(int slot_index, int type_id, int two_blocks);
void func_00124C60(Entity *entity);
void func_00124CF8(Entity *entity);

/* Gives each entity a pool block and queues its load; out-of-range entities without a block are skipped. */
void func_001232C0(void) {
    Entity *entity = D_004470A8;
    while (entity != NULL) {
        Entity *queued;
        float distance;
        float height_difference;

        distance = func_00124DE0(entity);
        entity->distance_to_player = distance;
        height_difference = ABS(D_00413F80[1] - entity->spawn_info->spawn_y);
        if (entity->unkD3 >= 0 ||
            (distance <= (float)entity->spawn_info->despawn_range &&
             (entity->spawn_info->vertical_despawn_range == 0 ||
              height_difference < (float)entity->spawn_info->vertical_despawn_range))) {
            entity->unkD3 = func_00123AF0(entity->spawn_info->type_id);
            // match: the slot is tested as a signed byte, as unkD3 is stored
            if ((char)entity->unkD3 >= 0) {
                queued = entity;
                entity = entity->next;
                if (func_00123918(queued)) {
                    func_00124C08(queued);
                    func_00124CF8(queued);
                }
                continue;
            }
            // match: pool_size is read unsigned (lbu) for this compare
            if ((u8)entity->spawn_info->pool_size < 5) {
                if (ENTITY_CONTROLLER(entity)->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                    entity->unkD3 = func_00123ED8(entity->spawn_info->pool_size);
                    if (entity->unkD3 >= 0) {
                        D_003D6DF0[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                        D_003D6DF0[entity->unkD3 + 1].head.word |= ENTITY_POOL_FLAG_200;
                    }
                } else {
                    entity->unkD3 = func_00123DD8(entity->spawn_info->pool_size);
                    if (entity->unkD3 >= 0) {
                        D_003D6DF0[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                    }
                }
            } else if (ENTITY_CONTROLLER(entity)->flags & CONTROLLER_FLAG_TWO_BLOCKS) {
                entity->unkD3 = func_00123C30();
                if (entity->unkD3 >= 0) {
                    D_003D6DF0[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                    D_003D6DF0[entity->unkD3 + 1].head.word |= ENTITY_POOL_FLAG_200;
                }
            } else {
                entity->unkD3 = func_00123B40();
                if (entity->unkD3 >= 0) {
                    D_003D6DF0[entity->unkD3].head.word |= ENTITY_POOL_FLAG_200;
                }
            }
            if (entity->unkD3 >= 0) {
                queued = entity;
                entity = entity->next;
                func_00124318(queued->unkD3, queued->spawn_info->type_id, ENTITY_CONTROLLER(queued)->flags >> 15);
                func_00124C08(queued);
                func_00124C60(queued);
                continue;
            }
        }
        entity = entity->next;
    }
}
