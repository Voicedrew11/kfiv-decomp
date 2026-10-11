#include "kf_entity.h"

int func_00123918(Entity *);
void func_00124CF8(Entity *);

/* Walks the D_004470B0 list and finishes each entity whose pool block is ready. */
void func_001234F0(void) {
    Entity *entity;
    Entity *current;

    entity = D_004470B0;
    while (entity != NULL) {
        if (D_003D6DF0[entity->unkD3].head.fields.kind == ENTITY_POOL_READY) {
            current = entity;
            entity = entity->next;
            if (func_00123918(current)) {
                func_00124CA0(current);
                func_00124CF8(current);
            }
        } else {
            entity = entity->next;
        }
    }
}
