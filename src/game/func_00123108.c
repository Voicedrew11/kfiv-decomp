#include "kf_entity.h"

/* Returns TRUE while any pool block is held by a loading or unpacking job. */
int func_00123108(void) {
    int i;
    int kind;

    for (i = 0; i < ENTITY_POOL_SLOT_COUNT; i++) {
        kind = D_003D6DF0[i].head.fields.kind;
        if (kind != ENTITY_POOL_FREE && kind != ENTITY_POOL_READY && kind != ENTITY_POOL_EXTENSION) {
            return TRUE;
        }
    }
    return FALSE;
}
